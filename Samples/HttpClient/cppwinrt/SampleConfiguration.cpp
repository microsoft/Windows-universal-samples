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
using namespace winrt::SDKTemplate;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
//using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::Web;
using namespace winrt::Windows::Web::Http;
//using namespace winrt::Windows::Web::Http::Filters;
//using namespace winrt::Windows::Web::Http::Headers;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"HttpClient C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"GET Text With Cache Control", xaml_typename<SDKTemplate::Scenario01_GetText>() },
    Scenario{ L"GET Stream", xaml_typename<SDKTemplate::Scenario02_GetStream>() },
    Scenario{ L"GET List", xaml_typename<SDKTemplate::Scenario03_GetList>() },
    Scenario{ L"POST Text", xaml_typename<SDKTemplate::Scenario04_PostText>() },
    Scenario{ L"POST Stream", xaml_typename<SDKTemplate::Scenario05_PostStream>() },
    Scenario{ L"POST Multipart", xaml_typename<SDKTemplate::Scenario06_PostMultipart>() },
    Scenario{ L"POST Stream With Progress", xaml_typename<SDKTemplate::Scenario07_PostStreamWithProgress>() },
    Scenario{ L"POST Custom Content", xaml_typename<SDKTemplate::Scenario08_PostCustomContent>() },
    Scenario{ L"Get Cookies", xaml_typename<SDKTemplate::Scenario09_GetCookie>() },
    Scenario{ L"Set Cookie", xaml_typename<SDKTemplate::Scenario10_SetCookie>() },
    Scenario{ L"Delete Cookie", xaml_typename<SDKTemplate::Scenario11_DeleteCookie>() },
    Scenario{ L"Disable Cookies", xaml_typename<SDKTemplate::Scenario12_DisableCookies>() },
    Scenario{ L"Retry Filter", xaml_typename<SDKTemplate::Scenario13_RetryFilter>() },
    Scenario{ L"Metered Connection Filter", xaml_typename<SDKTemplate::Scenario14_MeteredConnectionFilter>() },
    Scenario{ L"Server Certificate Validation", xaml_typename<SDKTemplate::Scenario15_ServerCertificateValidation>() },
});

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

hstring winrt::to_hstring(HttpResponseMessageSource value)
{
    switch (value)
    {
    case HttpResponseMessageSource::None: return L"None";
    case HttpResponseMessageSource::Cache: return L"Cache";
    case HttpResponseMessageSource::Network: return L"Network";
    }
    return to_hstring(static_cast<int32_t>(value));
}

hstring winrt::to_hstring(HttpVersion value)
{
    switch (value)
    {
    case HttpVersion::None: return L"None";
    case HttpVersion::Http10: return L"HTTP 1.0";
    case HttpVersion::Http11: return L"HTTP 1.1";
    case HttpVersion::Http20: return L"HTTP 2.0";
    }
    return to_hstring(static_cast<int32_t>(value));
}

hstring winrt::to_hstring(HttpProgressStage value)
{
    switch (value)
    {
    case HttpProgressStage::None: return L"None";
    case HttpProgressStage::DetectingProxy: return L"Detecting proxy";
    case HttpProgressStage::ResolvingName: return L"Resolving name";
    case HttpProgressStage::ConnectingToServer: return L"Connecting to server";
    case HttpProgressStage::NegotiatingSsl: return L"Negotiating SSL";
    case HttpProgressStage::SendingHeaders: return L"Sending headers";
    case HttpProgressStage::SendingContent: return L"Sending content";
    case HttpProgressStage::WaitingForResponse: return L"Waiting for response";
    case HttpProgressStage::ReceivingHeaders: return L"Receiving headers";
    case HttpProgressStage::ReceivingContent: return L"Receiving content";
    }
    return to_hstring(static_cast<int32_t>(value));
}
