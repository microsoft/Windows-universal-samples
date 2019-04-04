// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario7_DeviceInformationKind.g.h"
#include "MainPage.xaml.h"
#include "DisplayHelpers.h"
#include "DeviceWatcherHelper.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class Scenario7 sealed
    {
    public:
        Scenario7();

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
