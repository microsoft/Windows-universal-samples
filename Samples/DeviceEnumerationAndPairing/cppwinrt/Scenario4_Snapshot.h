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

#include "Scenario4_Snapshot.g.h"
#include "MainPage.h"
#include "DeviceWatcherHelper.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario4_Snapshot : Scenario4_SnapshotT<Scenario4_Snapshot>
    {
        Scenario4_Snapshot();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget FindButton_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ SDKTemplate::implementation::MainPage::Current() };
        Windows::Foundation::Collections::IObservableVector<SDKTemplate::DeviceInformationDisplay> resultCollection = winrt::single_threaded_observable_vector<SDKTemplate::DeviceInformationDisplay>();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario4_Snapshot : Scenario4_SnapshotT<Scenario4_Snapshot, implementation::Scenario4_Snapshot>
    {
    };
}
