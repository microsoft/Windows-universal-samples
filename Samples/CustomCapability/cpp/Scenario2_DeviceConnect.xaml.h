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
#include "Scenario2_DeviceConnect.g.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class DeviceConnect sealed
    {
    public:
        DeviceConnect();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage = MainPage::Current;

        void deviceConnectStart_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void deviceConnectStop_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void deviceConnectDevices_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);

        void UpdateStartStopButtons(void);

        Concurrency::task<void> OpenFx2DeviceAsync(Platform::String^ Id);
        void CloseFx2Device(void);
    };
}
