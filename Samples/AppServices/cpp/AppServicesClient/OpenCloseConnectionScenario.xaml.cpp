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
#include "OpenCloseConnectionScenario.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::AppService;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;

OpenCloseConnectionScenario::OpenCloseConnectionScenario()
{
    InitializeComponent();
}

void OpenCloseConnectionScenario::GenerateRandomNumber_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
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

    // Setup a new app service connection
    AppServiceConnection^ connection = ref new AppServiceConnection();
    connection->AppServiceName = "com.microsoft.randomnumbergenerator";
    connection->PackageFamilyName = "Microsoft.SDKSamples.AppServicesProvider.CPP_8wekyb3d8bbwe";

    // open the connection
    create_task(connection->OpenAsync()).then([this, minValueInput, maxValueInput, connection](AppServiceConnectionStatus status)
    {
        switch (status)
        {
        case AppServiceConnectionStatus::Success:
            // The new connection opened successfully
            rootPage->NotifyUser("Connection established", NotifyType::StatusMessage);
            break;

        case AppServiceConnectionStatus::AppNotInstalled:
            rootPage->NotifyUser("The app AppServicesProvider is not installed. Deploy AppServicesProvider to this device and try again.", NotifyType::ErrorMessage);
            return;

        case AppServiceConnectionStatus::AppUnavailable:
            rootPage->NotifyUser("The app AppServicesProvider is not available. This could be because it is currently being updated or was installed to a removable device that is no longer available.", NotifyType::ErrorMessage);
            return;

        case AppServiceConnectionStatus::AppServiceUnavailable:
            rootPage->NotifyUser("The app AppServicesProvider is installed but it does not provide the app service " + connection->AppServiceName, NotifyType::ErrorMessage);
            return;

        default:
        case AppServiceConnectionStatus::Unknown:
            rootPage->NotifyUser("An unkown error occurred while we were trying to open an AppServiceConnection.", NotifyType::ErrorMessage);
            return;
        }

        // Set up the inputs and send a message to the service
        auto inputs = ref new ValueSet();
        inputs->Insert("minvalue", minValueInput);
        inputs->Insert("maxvalue", maxValueInput);

        create_task(connection->SendMessageAsync(inputs)).then([this](AppServiceResponse^ response)
        {
            // If the service responded with success display the result and walk away
            if (response->Status == AppServiceResponseStatus::Success && response->Message->HasKey("result"))
            {
                auto resultText = response->Message->Lookup("result")->ToString();
                if (resultText != nullptr && !resultText->IsEmpty())
                {
                    Result->Text = resultText;
                    rootPage->NotifyUser("App service responded with a result", NotifyType::StatusMessage);
                }
                else
                {
                    rootPage->NotifyUser("App service did not respond with a result", NotifyType::ErrorMessage);
                }

                return;
            }

            // Something went wrong. Show the user a meaningful message depending upon the status
            switch (response->Status)
            {
            case AppServiceResponseStatus::Failure:
                rootPage->NotifyUser("The service failed to acknowledge the message we sent it. It may have been terminated or it's RequestReceived handler might not be handling incoming messages correctly.", NotifyType::ErrorMessage);
                return;

            case AppServiceResponseStatus::ResourceLimitsExceeded:
                rootPage->NotifyUser("The service exceeded the resources allocated to it and had to be terminated.", NotifyType::ErrorMessage);
                return;

            default:
            case AppServiceResponseStatus::Unknown:
                rootPage->NotifyUser("An unknown error occurred while we were trying to send a message to the service.", NotifyType::ErrorMessage);
                return;
            }
        }); // create_task(connection->SendMessageAsync(inputs))
    }); // create_task(connection->OpenAsync())
} // OpenCloseConnectionScenario::GenerateRandomNumber_Click
