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
#include "KeepConnectionOpenScenario.h"
#include "KeepConnectionOpenScenario.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::AppService;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    KeepConnectionOpenScenario::KeepConnectionOpenScenario()
    {
        InitializeComponent();
    }

    void KeepConnectionOpenScenario::OnNavigatedFrom(NavigationEventArgs const&)
    {
        //Clean up before we go
        if (connection != nullptr)
        {
            connection.Close();
            connection = nullptr;
        }
    }

    fire_and_forget KeepConnectionOpenScenario::OpenConnection_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        //Is a connection already open?
        if (connection != nullptr)
        {
            rootPage.NotifyUser(L"A connection already exists", NotifyType::ErrorMessage);
            co_return;
        }

        //Set up a new app service connection
        connection = AppServiceConnection();
        connection.AppServiceName(L"com.microsoft.randomnumbergenerator");
        connection.PackageFamilyName(L"Microsoft.SDKSamples.AppServicesProvider.CPPWINRT_8wekyb3d8bbwe");
        connection.ServiceClosed({ get_weak(), &KeepConnectionOpenScenario::Connection_ServiceClosed });
        AppServiceConnectionStatus status = co_await connection.OpenAsync();

        //"connection" may have been nulled out while we were awaiting.
        if (connection == nullptr)
        {
            rootPage.NotifyUser(L"Connection was closed", NotifyType::ErrorMessage);
            co_return;
        }

        //If the new connection opened successfully we're done here
        if (status == AppServiceConnectionStatus::Success)
        {
            rootPage.NotifyUser(L"Connection is open", NotifyType::StatusMessage);
        }
        else
        {
            //Something went wrong. Lets figure out what it was and show the 
            //user a meaningful message
            switch (status)
            {
            case AppServiceConnectionStatus::AppNotInstalled:
                rootPage.NotifyUser(L"The app AppServicesProvider is not installed. Deploy AppServicesProvider to this device and try again.", NotifyType::ErrorMessage);
                break;

            case AppServiceConnectionStatus::AppUnavailable:
                rootPage.NotifyUser(L"The app AppServicesProvider is not available. This could be because it is currently being updated or was installed to a removable device that is no longer available.", NotifyType::ErrorMessage);
                break;

            case AppServiceConnectionStatus::AppServiceUnavailable:
                rootPage.NotifyUser(L"The app AppServicesProvider is installed but it does not provide the app service " + connection.AppServiceName(), NotifyType::ErrorMessage);
                break;

            default:
            case AppServiceConnectionStatus::Unknown:
                rootPage.NotifyUser(L"An unknown error occurred while we were trying to open an AppServiceConnection.", NotifyType::ErrorMessage);
                break;
            }

            //Clean up before we go
            connection.Close();
            connection = nullptr;
        }
    }

    fire_and_forget KeepConnectionOpenScenario::Connection_ServiceClosed(AppServiceConnection const&, AppServiceClosedEventArgs const&)
    {
        auto lifetime = get_strong();

        co_await resume_foreground(Dispatcher());

        //Close the connection reference we're holding
        if (connection != nullptr)
        {
            connection.Close();
            connection = nullptr;
        }
    }

    void KeepConnectionOpenScenario::CloseConnection_Click(IInspectable const&, RoutedEventArgs const&)
    {
        //Is there an open connection?
        if (connection == nullptr)
        {
            rootPage.NotifyUser(L"There's no open connection to close", NotifyType::ErrorMessage);
            return;
        }

        //Close the open connection
        connection.Close();
        connection = nullptr;

        //Let the user know we closed the connection
        rootPage.NotifyUser(L"Connection is closed", NotifyType::StatusMessage);
    }

    fire_and_forget KeepConnectionOpenScenario::GenerateRandomNumber_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        //Is there an open connection?
        if (connection == nullptr)
        {
            rootPage.NotifyUser(L"You need to open a connection before trying to generate a random number.", NotifyType::ErrorMessage);
            co_return;
        }

        //Parse user input
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

        //Send a message to the app service
        ValueSet inputs;
        inputs.Insert(L"minvalue", box_value(minValueInput));
        inputs.Insert(L"maxvalue", box_value(maxValueInput));
        AppServiceResponse response = co_await connection.SendMessageAsync(inputs);

        //"connection" may have been nulled out while we were awaiting.
        if (connection == nullptr)
        {
            rootPage.NotifyUser(L"Connection was closed", NotifyType::ErrorMessage);
            co_return;
        }

        //If the service responded display the message. We're done!
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
        }
        else
        {
            //Something went wrong while sending a message. Let display
            //a meaningful error message
            switch (response.Status())
            {
            case AppServiceResponseStatus::Failure:
                rootPage.NotifyUser(L"The service failed to acknowledge the message we sent it. It may have been terminated because the client was suspended.", NotifyType::ErrorMessage);
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
}
