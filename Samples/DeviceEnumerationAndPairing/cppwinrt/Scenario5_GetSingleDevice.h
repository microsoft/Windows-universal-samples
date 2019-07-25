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

#include "Scenario5_GetSingleDevice.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario5_GetSingleDevice : Scenario5_GetSingleDeviceT<Scenario5_GetSingleDevice>
    {
        Scenario5_GetSingleDevice();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget Page_Loaded(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget GetButton_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ SDKTemplate::implementation::MainPage::Current() };
        Windows::Devices::Enumeration::DeviceInformationKind deviceInformationKind;
        Windows::Foundation::Collections::IObservableVector<SDKTemplate::DeviceInformationDisplay> resultCollection = winrt::single_threaded_observable_vector<SDKTemplate::DeviceInformationDisplay>();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario5_GetSingleDevice : Scenario5_GetSingleDeviceT<Scenario5_GetSingleDevice, implementation::Scenario5_GetSingleDevice>
    {
    };
}
