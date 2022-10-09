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

#pragma once

#include "KeepConnectionOpenScenario.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct KeepConnectionOpenScenario : KeepConnectionOpenScenarioT<KeepConnectionOpenScenario>
    {
        KeepConnectionOpenScenario();

        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget OpenConnection_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void CloseConnection_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget GenerateRandomNumber_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::ApplicationModel::AppService::AppServiceConnection connection{ nullptr };

        fire_and_forget Connection_ServiceClosed(Windows::ApplicationModel::AppService::AppServiceConnection const&, Windows::ApplicationModel::AppService::AppServiceClosedEventArgs const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct KeepConnectionOpenScenario : KeepConnectionOpenScenarioT<KeepConnectionOpenScenario, implementation::KeepConnectionOpenScenario>
    {
    };
}
