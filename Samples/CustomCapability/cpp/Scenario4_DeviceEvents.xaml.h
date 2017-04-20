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
#include "Scenario4_DeviceEvents.g.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class DeviceEvents sealed
    {
    public:
        DeviceEvents();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage = MainPage::Current;

        Concurrency::cancellation_token_source cancelSource;
        bool running = false;

        Windows::Foundation::EventRegistrationToken switchChangedEventsHandler;
        Platform::Array<bool>^ previousSwitchValues;

        void deviceEventsGet_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void deviceEventsBegin_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void deviceEventsCancel_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void StartInterruptMessageWorker(Windows::Devices::Custom::CustomDevice^ fx2Device);
        void InterruptMessageWorker(Windows::Devices::Custom::CustomDevice^ fx2Device,
                                    Windows::Storage::Streams::IBuffer^ switchMessageBuffer);

        void UpdateRegisterButton();

        void ClearSwitchStateTable();
        void UpdateSwitchStateTable(Platform::Array<bool>^ SwitchStateArray);

        Platform::Array<bool>^ CreateSwitchStateArray(byte output[])
        {
            auto switchStateArray = ref new Platform::Array<bool>(8);

            for (auto i = 0; i < 8; i += 1)
            {
                switchStateArray[i] = (output[0] & (1 << i)) != 0;
            }

            return switchStateArray;
        }
    };
}
