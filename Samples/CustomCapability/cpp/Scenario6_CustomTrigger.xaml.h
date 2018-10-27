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

#include "Scenario6_CustomTrigger.g.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class CustomTrigger sealed
    {
    public:
        CustomTrigger();

    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::ApplicationModel::Background::BackgroundTaskRegistration^ taskRegistration;

        bool IsFx2CustomTriggerTaskRegistered();
        void UpdateUI();
        void UnregisterTask();

        void RegisterCustomTrigger_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void UnregisterCustomTrigger_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

    protected:
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    };
}
