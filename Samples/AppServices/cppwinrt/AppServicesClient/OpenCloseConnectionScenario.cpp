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
#include "OpenCloseConnectionScenario.h"
#include "OpenCloseConnectionScenario.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::AppService;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    OpenCloseConnectionScenario::OpenCloseConnectionScenario()
    {
        InitializeComponent();
    }

    fire_and_forget OpenCloseConnectionScenario::GenerateRandomNumber_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Parse user input.
        int minValueInput = 0;
        bool valueParsed = TryParseInt32(MinValue().Text().c_str(), minValueInput);
        if (!valueParsed)
        {
            rootPage.NotifyUser(L"The Minimum Value should be a valid integer", NotifyType::ErrorMessage);
            co_return;
        }

        int maxValueInput = 0;
        valueParsed = TryParseInt32(MaxValue().Text().c_str(), maxValueInput);
        if (!valueParsed)
        {
            rootPage.NotifyUser(L"The Maximum Value should be a valid integer", NotifyType::ErrorMessage);
            co_return;
        }

        if (maxValueInput <= minValueInput)
        {
            rootPage.NotifyUser(L"Maximum Value must be larger than Minimum Value", NotifyType::ErrorMessage);
            co_return;
        }

        AppServiceConnection connection;
        ensure_close close_connection(connection);

        //Set up a new app service connection
        connection.AppServiceName(L"com.microsoft.randomnumbergenerator");
        connection.PackageFamilyName(L"Microsoft.SDKSamples.AppServicesProvider.CPPWINRT_8wekyb3d8bbwe");
        AppServiceConnectionStatus status = co_await connection.OpenAsync();

        switch (status)
        {
        case AppServiceConnectionStatus::Success:
            // The new connection opened successfully
            rootPage.NotifyUser(L"Connection established", NotifyType::StatusMessage);
            break;

        case AppServiceConnectionStatus::AppNotInstalled:
            rootPage.NotifyUser(L"The app AppServicesProvider is not installed. Deploy AppServicesProvider to this device and try again.", NotifyType::ErrorMessage);
            co_return;

        case AppServiceConnectionStatus::AppUnavailable:
            rootPage.NotifyUser(L"The app AppServicesProvider is not available. This could be because it is currently being updated or was installed to a removable device that is no longer available.", NotifyType::ErrorMessage);
            co_return;

        case AppServiceConnectionStatus::AppServiceUnavailable:
            rootPage.NotifyUser(L"The app AppServicesProvider is installed but it does not provide the app service " + connection.AppServiceName(), NotifyType::ErrorMessage);
            co_return;

        default:
        case AppServiceConnectionStatus::Unknown:
            rootPage.NotifyUser(L"An unknown error occurred while we were trying to open an AppServiceConnection.", NotifyType::ErrorMessage);
            co_return;
        }

        //Set up the inputs and send a message to the service
        ValueSet inputs;
        inputs.Insert(L"minvalue", box_value(minValueInput));
        inputs.Insert(L"maxvalue", box_value(maxValueInput));
        AppServiceResponse response = co_await connection.SendMessageAsync(inputs);

        //If the service responded with success display the result and walk away
        if (response.Status() == AppServiceResponseStatus::Success)
        {
            std::optional<int> result = response.Message().TryLookup(L"result").try_as<int>();
            if (result.has_value())
            {
                Result().Text(to_hstring(result.value()));
                rootPage.NotifyUser(L"App service responded with a result", NotifyType::StatusMessage);
            }
            else
            {
                rootPage.NotifyUser(L"App service did not respond with a result", NotifyType::ErrorMessage);
            }

            co_return;
        }

        //Something went wrong while sending a message. Let display
        //a meaningful error message
        switch (response.Status())
        {
        case AppServiceResponseStatus::Failure:
            rootPage.NotifyUser(L"The service failed to acknowledge the message we sent it. It may have been terminated or its RequestReceived handler might not be handling incoming messages correctly.", NotifyType::ErrorMessage);
            co_return;

        case AppServiceResponseStatus::ResourceLimitsExceeded:
            rootPage.NotifyUser(L"The service exceeded the resources allocated to it and had to be terminated.", NotifyType::ErrorMessage);
            co_return;

        default:
        case AppServiceResponseStatus::Unknown:
            rootPage.NotifyUser(L"An unknown error occurred while we were trying to send a message to the service.", NotifyType::ErrorMessage);
            co_return;
        }
    }
}
