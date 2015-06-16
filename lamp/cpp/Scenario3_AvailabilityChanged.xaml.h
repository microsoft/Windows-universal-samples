//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include "Scenario3_AvailabilityChanged.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_AvailabilityChanged sealed
    {
    public:
        Scenario3_AvailabilityChanged();
        
    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;

    private:
        MainPage^ _rootPage;
        Windows::Devices::Lights::Lamp^ _lamp;
        Windows::Foundation::EventRegistrationToken _lampAvailabilutyChangedEventToken;

        Concurrency::task<void> InitializeLampAsync();
        void DisposeLamp();

        void Lamp_AvailabilityChanged(Windows::Devices::Lights::Lamp^ sender, Windows::Devices::Lights::LampAvailabilityChangedEventArgs^ args);
        void RegisterBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void UnRegisterBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void lampToggle_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        Concurrency::task<void> LogStatusToOutputBoxAsync(Platform::String^ message);
        Concurrency::task<void> LogStatusAsync(Platform::String^ message, NotifyType type);
    };
}
