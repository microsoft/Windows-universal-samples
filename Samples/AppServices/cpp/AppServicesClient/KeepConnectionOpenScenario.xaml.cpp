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
#include "KeepConnectionOpenScenario.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::AppService;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;

KeepConnectionOpenScenario::KeepConnectionOpenScenario()
{
    InitializeComponent();
}

void KeepConnectionOpenScenario::OpenConnection_Click(Object^ sender, RoutedEventArgs^ e)
{
    //Is a connection already open?
    if (_connection != nullptr)
    {
        rootPage->NotifyUser("A connection already exists", NotifyType::ErrorMessage);
        return;
    }

    //Set up a new app service connection
    _connection = ref new AppServiceConnection();
    _connection->AppServiceName = "com.microsoft.randomnumbergenerator";
    _connection->PackageFamilyName = "Microsoft.SDKSamples.AppServicesProvider.CPP_8wekyb3d8bbwe";
    _connection->ServiceClosed += ref new TypedEventHandler<AppServiceConnection^, AppServiceClosedEventArgs^>(this, &KeepConnectionOpenScenario::Connection_ServiceClosed);

    // open the connection
    create_task(_connection->OpenAsync()).then([this](AppServiceConnectionStatus status)
    {
        //If the new connection opened successfully we're done here
        if (status == AppServiceConnectionStatus::Success)
        {
            rootPage->NotifyUser("Connection is open", NotifyType::StatusMessage);
        }
        else
        {
            // Something went wrong. Show the user a meaningful message.
            switch (status)
            {
            case AppServiceConnectionStatus::AppNotInstalled:
                rootPage->NotifyUser("The app AppServicesProvider is not installed. Deploy AppServicesProvider to this device and try again.", NotifyType::ErrorMessage);
                break;

            case AppServiceConnectionStatus::AppUnavailable:
                rootPage->NotifyUser("The app AppServicesProvider is not available. This could be because it is currently being updated or was installed to a removable device that is no longer available.", NotifyType::ErrorMessage);
                break;

            case AppServiceConnectionStatus::AppServiceUnavailable:
                rootPage->NotifyUser("The app AppServicesProvider is installed but it does not provide the app service " + _connection->AppServiceName, NotifyType::ErrorMessage);
                break;

            default:
            case AppServiceConnectionStatus::Unknown:
                rootPage->NotifyUser("An unknown error occurred while we were trying to open an AppServiceConnection.", NotifyType::ErrorMessage);
                break;
            }

            // Dispose the connection
            delete _connection;
            _connection = nullptr;
        }
    }); // create_task()
} // OpenConnection_Click() 

void KeepConnectionOpenScenario::Connection_ServiceClosed(AppServiceConnection^ sender, AppServiceClosedEventArgs^ args)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
    {
        // Dispose the connection
        delete _connection;
        _connection = nullptr;
    }));
}

void KeepConnectionOpenScenario::CloseConnection_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Is there an open connection?
    if (_connection == nullptr)
    {
        rootPage->NotifyUser("There's no open connection to close", NotifyType::ErrorMessage);
        return;
    }

    // Close the open connection
    delete _connection;
    _connection = nullptr;

    // Let the user know we closed the connection
    rootPage->NotifyUser("Connection is closed", NotifyType::StatusMessage);
}

void KeepConnectionOpenScenario::GenerateRandomNumber_Click(Object^ sender, RoutedEventArgs^ e)
{
    //  Is there an open connection?
    if (_connection == nullptr)
    {
        rootPage->NotifyUser("You need to open a connection before trying to generate a random number.", NotifyType::ErrorMessage);
        return;
    }

    // Parse user input
    int32_t minValueInput;
    if (!TryParseInt(MinValue->Text->Data(), &minValueInput))
    {
        rootPage->NotifyUser("The Minimum Value should be a valid integer", NotifyType::ErrorMessage);
        return;
    }

    int32_t maxValueInput;
    if (!TryParseInt(MaxValue->Text->Data(), &maxValueInput))
    {
        rootPage->NotifyUser("The Maximum Value should be a valid integer", NotifyType::ErrorMessage);
        return;
    }

    if (maxValueInput <= minValueInput)
    {
        rootPage->NotifyUser("Maximum Value must be larger than Minimum Value", NotifyType::ErrorMessage);
        return;
    }

    // Send a message to the app service
    auto inputs = ref new ValueSet();
    inputs->Insert("minvalue", minValueInput);
    inputs->Insert("maxvalue", maxValueInput);

    create_task(_connection->SendMessageAsync(inputs)).then([this](AppServiceResponse^ response)
    {
        // If the service responded display the message. We're done!
        if (response->Status == AppServiceResponseStatus::Success)
        {
            if (!response->Message->HasKey("result"))
            {
                rootPage->NotifyUser("The app service response message does not contain a key called \"result\"", NotifyType::StatusMessage);
                return;
            }

            auto resultText = response->Message->Lookup("result")->ToString();
            if (!resultText->IsEmpty())
            {
                Result->Text = resultText;
                rootPage->NotifyUser("App service responded with a result", NotifyType::StatusMessage);
            }
            else
            {
                rootPage->NotifyUser("App service did not respond with a result", NotifyType::ErrorMessage);
            }
        }
        else
        {
            // Something went wrong. Show the user a meaningful message.
            switch (response->Status)
            {
            case AppServiceResponseStatus::Failure:
                rootPage->NotifyUser("The service failed to acknowledge the message we sent it. It may have been terminated because the client was suspended.", NotifyType::ErrorMessage);
                break;

            case AppServiceResponseStatus::ResourceLimitsExceeded:
                rootPage->NotifyUser("The service exceeded the resources allocated to it and had to be terminated.", NotifyType::ErrorMessage);
                break;

            default:
            case AppServiceResponseStatus::Unknown:
                rootPage->NotifyUser("An unknown error occurred while we were trying to send a message to the service.", NotifyType::ErrorMessage);
                break;
            }
        }
    }); // create_task().then()
} // GenerateRandomNumber_Click