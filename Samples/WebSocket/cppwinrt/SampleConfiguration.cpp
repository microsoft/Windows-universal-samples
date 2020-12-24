//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Security::Cryptography::Certificates;
using namespace winrt::Windows::Web;
using namespace winrt::SDKTemplate;
using namespace std::literals::chrono_literals;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"WebSocket C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"UTF-8 text messages", xaml_typename<SDKTemplate::Scenario1_UTF8>() },
    Scenario{ L"Binary data stream", xaml_typename<SDKTemplate::Scenario2_Binary>() },
    Scenario{ L"Client authentication", xaml_typename<SDKTemplate::Scenario3_ClientAuthentication>() },
    Scenario{ L"Partial and Complete Messages", xaml_typename<SDKTemplate::Scenario4_PartialReadWrite>() },
});

hstring winrt::to_hstring(SocketMessageType value)
{
    switch (value)
    {
    case SocketMessageType::Binary: return L"Binary";
    case SocketMessageType::Utf8: return L"Utf8";
    }
    return to_hstring(static_cast<int32_t>(value));
}

hstring winrt::to_hstring(WebErrorStatus value)
{
    switch (value)
    {
    case WebErrorStatus::Unknown: return L"Unknown";
    case WebErrorStatus::CertificateCommonNameIsIncorrect: return L"CertificateCommonNameIsIncorrect";
    case WebErrorStatus::CertificateExpired: return L"CertificateExpired";
    case WebErrorStatus::CertificateContainsErrors: return L"CertificateContainsErrors";
    case WebErrorStatus::CertificateRevoked: return L"CertificateRevoked";
    case WebErrorStatus::CertificateIsInvalid: return L"CertificateIsInvalid";
    case WebErrorStatus::ServerUnreachable: return L"ServerUnreachable";
    case WebErrorStatus::Timeout: return L"Timeout";
    case WebErrorStatus::ErrorHttpInvalidServerResponse: return L"ErrorHttpInvalidServerResponse";
    case WebErrorStatus::ConnectionAborted: return L"ConnectionAborted";
    case WebErrorStatus::ConnectionReset: return L"ConnectionReset";
    case WebErrorStatus::Disconnected: return L"Disconnected";
    case WebErrorStatus::HttpToHttpsOnRedirection: return L"HttpToHttpsOnRedirection";
    case WebErrorStatus::HttpsToHttpOnRedirection: return L"HttpsToHttpOnRedirection";
    case WebErrorStatus::CannotConnect: return L"CannotConnect";
    case WebErrorStatus::HostNameNotResolved: return L"HostNameNotResolved";
    case WebErrorStatus::OperationCanceled: return L"OperationCanceled";
    case WebErrorStatus::RedirectFailed: return L"RedirectFailed";
    case WebErrorStatus::UnexpectedStatusCode: return L"UnexpectedStatusCode";
    case WebErrorStatus::UnexpectedRedirection: return L"UnexpectedRedirection";
    case WebErrorStatus::UnexpectedClientError: return L"UnexpectedClientError";
    case WebErrorStatus::UnexpectedServerError: return L"UnexpectedServerError";
    case WebErrorStatus::InsufficientRangeSupport: return L"InsufficientRangeSupport";
    case WebErrorStatus::MissingContentLengthSupport: return L"MissingContentLengthSupport";
    }
    return to_hstring(static_cast<int32_t>(value));
}

Uri winrt::SDKTemplate::TryGetUri(MainPage const& rootPage, hstring const& uriString)
{
    Uri webSocketUri{ nullptr };

    try
    {
        webSocketUri = Uri(uriString);
    }
    catch (hresult_error const&)
    {
        rootPage.NotifyUser(L"Error: Invalid URI", NotifyType::ErrorMessage);
        return nullptr;
    }

    // Fragments are not allowed in WebSocket URIs.
    if (!webSocketUri.Fragment().empty())
    {
        rootPage.NotifyUser(L"Error: URI fragments not supported in WebSocket URIs.", NotifyType::ErrorMessage);
        return nullptr;
    }

    // Uri.SchemeName returns the canonicalized scheme name so we can use case-sensitive, ordinal string
    // comparison.
    if ((webSocketUri.SchemeName() != L"ws") && (webSocketUri.SchemeName() != L"wss"))
    {
        rootPage.NotifyUser(L"Error: WebSockets only support ws:// and wss:// schemes.", NotifyType::ErrorMessage);
        return nullptr;
    }

    return webSocketUri;
}

hstring winrt::SDKTemplate::BuildWebSocketError(hresult_error const& ex)
{
    if (ex.code() == INET_E_SECURITY_PROBLEM) // 0x800C000EU
    {
        // INET_E_SECURITY_PROBLEM - our custom certificate validator rejected the request.
        return L"Error: Rejected by custom certificate validation.";
    }

    WebErrorStatus status = WebSocketError::GetStatus(ex.code());

    // Normally we'd use the HResult and status to test for specific conditions we want to handle.
    // In this sample, we'll just output them for demonstration purposes.
    switch (status)
    {
    case WebErrorStatus::CannotConnect:
    case WebErrorStatus::NotFound:
    case WebErrorStatus::RequestTimeout:
        return L"Cannot connect to the server. Please make sure to run the server setup script before running the sample.";

    case WebErrorStatus::Unknown:
        return L"COM error: " + to_hstring(ex.code());

    default:
        return L"Error: " + to_hstring(status);
    }
}

IAsyncOperation<bool> IsCertificateValidAsync(Certificate serverCert)
{
    // This is a placeholder call to simulate long-running async calls. Note that this code runs synchronously as part
    // of the SSL/TLS handshake. Avoid performing lengthy operations here - else, the remote server may terminate the
    // connection abruptly.
    co_await winrt::resume_after(100ms);

    // In this sample, we check the issuer of the certificate - this is purely for illustration
    // purposes and should not be considered as a recommendation for certificate validation.
    co_return serverCert.Issuer() == L"www.fabrikam.com";
}

IAsyncOperation<bool> winrt::SDKTemplate::AreCertificateAndCertChainValidAsync(Certificate serverCert, IVectorView<Certificate> certChain)
{
    for (auto&& cert : certChain)
    {
        if (!(co_await IsCertificateValidAsync(cert)))
        {
            co_return false;
        }
    }
    co_return co_await IsCertificateValidAsync(serverCert);
}

