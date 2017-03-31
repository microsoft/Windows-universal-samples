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
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;
using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking::Sockets;
using namespace Windows::Security::Cryptography::Certificates;
using namespace Windows::Web;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>  
{
    { "UTF-8 text messages", "SDKTemplate.Scenario1" },
    { "Binary data stream", "SDKTemplate.Scenario2" }
}; 

Uri^ MainPage::TryGetUri(String^ uriString)
{
    Windows::Foundation::Uri^ webSocketUri;

    // Create a Uri instance and catch exceptions related to invalid input. This method returns 'true'
    // if the Uri instance was successfully created and 'false' otherwise.
    try
    {
        webSocketUri = ref new Uri(uriString);
    }
    catch (NullReferenceException^)
    {
        NotifyUser("Error: URI must not be null or empty.", NotifyType::ErrorMessage);
        return nullptr;
    }
    catch (InvalidArgumentException^)
    {
        NotifyUser("Error: Invalid URI", NotifyType::ErrorMessage);
        return nullptr;
    }

    if (webSocketUri->Fragment != "")
    {
        NotifyUser("Error: URI fragments not supported in WebSocket URIs.", NotifyType::ErrorMessage);
        return nullptr;
    }

    // Uri->SchemeName returns the canonicalized scheme name so we can use case-sensitive, ordinal string
    // comparison.
    if (webSocketUri->SchemeName != "ws" && webSocketUri->SchemeName != "wss")
    {
        NotifyUser("Error: WebSockets only support ws:// and wss:// schemes.", NotifyType::ErrorMessage);
        return nullptr;
    }

    return webSocketUri;
}

String^ MainPage::BuildWebSocketError(Exception^ ex)
{
    // Normally we'd use the HResult and status to test for specific conditions we want to handle.
    // In this sample, we'll just output them for demonstration purposes.

    WebErrorStatus status = WebSocketError::GetStatus(ex->HResult);

    switch (status)
    {
    case WebErrorStatus::CannotConnect:
    case WebErrorStatus::NotFound:
    case WebErrorStatus::RequestTimeout:
        return "Cannot connect to the server. Please make sure " +
            "to run the server setup script before running the sample.";

    case WebErrorStatus::Unknown:
        return "COM error: " + ex->HResult.ToString();

    default:
        return "Error: " + status.ToString();
    }
}

// All the certificates in the certificate chain as well as the final certificate itself
// must be valid.
task<bool> MainPage::AreCertificateAndCertChainValidAsync(
    Certificate^ serverCert,
    IVectorView<Windows::Security::Cryptography::Certificates::Certificate^>^ certChain)
{
    std::vector<task<bool>> tasks(certChain->Size + 1);
    // Check validity of all the certificates in the certificate chain.
    std::transform(begin(certChain), end(certChain), begin(tasks), IsCertificateValidAsync);
    // Check validity of the final certificate.
    tasks[certChain->Size] = IsCertificateValidAsync(serverCert);

    return when_all(begin(tasks), end(tasks)).then([](std::vector<bool> results)
    {
        // Return true if every result is true.
        return std::find(begin(results), end(results), false) == end(results);
    });
}

// This is a placeholder call to simulate long-running async calls. Note that this code runs synchronously as part  
// of the SSL/TLS handshake. Avoid performing lengthy operations here - else, the remote server may terminate the 
// connection abruptly. 
task<bool> MainPage::IsCertificateValidAsync(Certificate^ serverCert)
{
    // In this sample, we check the issuer of the certificate - this is purely for illustration 
    // purposes and should not be considered as a recommendation for certificate validation.
    return task_from_result(serverCert->Issuer == "www.fabrikam.com");
}
