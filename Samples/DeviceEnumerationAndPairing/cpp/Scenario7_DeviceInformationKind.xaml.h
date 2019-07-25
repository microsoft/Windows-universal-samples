// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario7_DeviceInformationKind.g.h"
#include "MainPage.xaml.h"
#include "DisplayHelpers.h"
#include "DeviceWatcherHelper.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario7_DeviceInformationKind sealed
    {
    public:
        Scenario7_DeviceInformationKind();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void StartWatchers();
        void StopWatchers(bool reset = false);

        void OnDeviceListChanged(Windows::Devices::Enumeration::DeviceWatcher^ sender, Platform::String^ id);

        MainPage^ rootPage = MainPage::Current;
        Windows::Foundation::Collections::IObservableVector<DeviceInformationDisplay^>^ resultCollection = ref new Platform::Collections::Vector<DeviceInformationDisplay^>();
        Platform::Collections::Vector<DeviceWatcherHelper^>^ deviceWatcherHelpers = ref new Platform::Collections::Vector<DeviceWatcherHelper^>();
    };
}
