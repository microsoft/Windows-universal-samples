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

//
// Scenario5.xaml.cpp
// Implementation of the Scenario5 class
//

#include "pch.h"
#include "Scenario5.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::StreamSocketSample;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Security::Cryptography::Certificates;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario5::Scenario5()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario5::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}


void Scenario5::ConnectSocket_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // By default 'HostNameForConnect' is disabled and host name validation is not required. When enabling the 
    // text box validating the host name is required since it was received from an untrusted source (user input).
    // The host name is validated by catching InvalidArgumentExceptions thrown by the HostName constructor for 
    // invalid input.
    // Note that when enabling the text box users may provide names for hosts on the intErnet that require the
    // "Internet (Client)" capability.
    if (ServiceNameForConnect->Text == nullptr)
    {
        rootPage->NotifyUser("Please provide a service name.", NotifyType::ErrorMessage);
        return;
    }

    HostName^ hostName;
    try
    {
        hostName = ref new HostName(HostNameForConnect->Text);
    }
    catch (InvalidArgumentException^ e)
    {
        rootPage->NotifyUser("Error: Invalid host name.", NotifyType::ErrorMessage);
        return;
    }

    ConnectSocketWithRetry(hostName, ServiceNameForConnect->Text);
}

void Scenario5::ConnectSocketWithRetry(HostName^ hostName, String^ serviceName)
{
    StreamSocket^ socket = ref new StreamSocket();
    rootPage->NotifyUser("Connecting to: " + HostNameForConnect->Text, NotifyType::StatusMessage);
    
    // Connect to the server (in our case the local IIS server).
    create_task(socket->ConnectAsync(hostName, serviceName, SocketProtectionLevel::Tls12)).then(
        [this, socket, hostName, serviceName] (task<void> previousTask) -> task<void>
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            previousTask.get();
            return create_task([]{});
        }
        catch (Exception^ exception)
        {
            // If the exception was caused by an SSL error that is ignorable we are going to prompt the user
            // with an enumeration of the errors and ask for permission to ignore.
            if (socket->Information->ServerCertificateErrorSeverity == SocketSslErrorSeverity::Ignorable && 
                socket->Information->ServerCertificateErrors->Size > 0)
            {
                return create_task(
                    ShouldIgnoreCertificateErrorsAsync(socket->Information->ServerCertificateErrors)).then(
                        [this, socket, hostName, serviceName] (bool userAcceptedRetry)
                {
                    if (userAcceptedRetry)
                    {
                        // Retry the connection, ignoring SSL certificate errors
                        return ConnectSocketWithRetryHandleSslErrorAsync(socket, hostName, serviceName);
                    }

                    cancel_current_task();
                });
            }

            // Handle all other exceptions in the last continuation
            throw;
        }
    }).then([this, socket](task<void> previousTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            previousTask.get();

            String^ certInformation = GetCertificateInformation(
                socket->Information->ServerCertificate,
                socket->Information->ServerIntermediateCertificates);

            rootPage->NotifyUser(
                "Connected to server. Certificate information: \r\n" + certInformation,
                NotifyType::StatusMessage);
        }
        catch (const task_canceled&)
        {
            rootPage->NotifyUser("Connection aborted by user.", NotifyType::ErrorMessage);
        }
        catch (Exception^ exception)
        {
            if (SocketError::GetStatus(exception->HResult) == SocketErrorStatus::Unknown)
            {
                throw;
            }

            rootPage->NotifyUser("Connect failed with error: " + exception->Message, NotifyType::ErrorMessage);
        }

        delete socket;
    });
}

task<void> Scenario5::ConnectSocketWithRetryHandleSslErrorAsync(
    StreamSocket^ socket, 
    HostName^ hostName, 
    String^ serviceName)
{
    // ---------------------------------------------------------------------------
    // WARNING: Only test applications may ignore SSL errors.
    // In real applications, ignoring server certificate errors can lead to MITM
    // attacks (while the connection is secure, the server is not authenticated).
    // ---------------------------------------------------------------------------
    socket->Control->IgnorableServerCertificateErrors->Clear();
    std::for_each(
        begin(socket->Information->ServerCertificateErrors),
        end(socket->Information->ServerCertificateErrors),
        [socket] (ChainValidationResult ignorableError)
        {
            socket->Control->IgnorableServerCertificateErrors->Append(ignorableError);
        });

    rootPage->NotifyUser("Retrying connection", NotifyType::StatusMessage);
    return create_task(socket->ConnectAsync(hostName, serviceName, SocketProtectionLevel::Tls12));
}

task<bool> Scenario5::ShouldIgnoreCertificateErrorsAsync(IVectorView<ChainValidationResult>^ serverCertificateErrors)
{
    String^ connectionErrors = serverCertificateErrors->GetAt(0).ToString();

    std::for_each(
        begin(serverCertificateErrors) + 1, 
        end(serverCertificateErrors), 
        [&connectionErrors] (ChainValidationResult ignorableError)
    {
        connectionErrors += ", " + ignorableError.ToString();
    });

    String^ dialogMessage =
        "The remote server certificate validation failed with the following errors: " +
        connectionErrors + "\r\nSecurity certificate problems may" +
        " indicate an attempt to fool you or intercept any data you send to the server.";

    MessageDialog^ dialog = ref new MessageDialog(dialogMessage, "Server Certificate Validation Errors");

    dialog->Commands->Append(
        ref new UICommand("Continue (not recommended)", nullptr, PropertyValue::CreateInt32(continueButtonId)));
    dialog->Commands->Append(ref new UICommand("Cancel", nullptr, PropertyValue::CreateInt32(abortButtonId)));
    dialog->DefaultCommandIndex = 1;
    dialog->CancelCommandIndex = 1;

    return create_task(dialog->ShowAsync()).then([] (IUICommand^ selected) -> bool
    {
        auto selectedId = dynamic_cast<IPropertyValue^>(selected->Id);
        return selectedId && (selectedId->GetInt32() == continueButtonId);
    });
}

String^ Scenario5::GetCertificateInformation(
    Certificate^ serverCert, 
    IVectorView<Certificate^>^ intermediateCertificates)
{
    String^ sb = "";
    sb += "\tFriendly Name: " + serverCert->FriendlyName + "\r\n";
    sb += "\tSubject: " + serverCert->Subject + "\r\n";
    sb += "\tIssuer: " + serverCert->Issuer + "\r\n";

    auto dateFormatter = ref new DateTimeFormatter("longdate");

    sb += "\tValidity: " + dateFormatter->Format(serverCert->ValidFrom) + " - " + 
        dateFormatter->Format(serverCert->ValidTo) + "\r\n";

    // Enumerate the entire certificate chain.
    if (intermediateCertificates->Size > 0)
    {
        sb += "\tCertificate chain: \r\n";

        std::for_each(begin(intermediateCertificates), end(intermediateCertificates), [&sb](Certificate^ cert)
        {
            sb += "\t\tIntermediate Certificate Subject: " + cert->Subject + "\r\n";
        });
    }
    else
    {
        sb += "\tNo certificates within the intermediate chain.";
    }

    return sb;
}
