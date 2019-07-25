// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario8_PairDevice.g.h"
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
    public ref class Scenario8_PairDevice sealed
    {
    public:
        Scenario8_PairDevice();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PairButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void UnpairButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void ResultsListView_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);

        void StartWatcher();
        void StopWatcher();

        void OnDeviceListChanged(Windows::Devices::Enumeration::DeviceWatcher^ sender, Platform::String^ id);
        void UpdatePairingButtons();

        MainPage^ rootPage = MainPage::Current;
        Windows::Foundation::Collections::IObservableVector<DeviceInformationDisplay^>^ resultCollection = ref new Platform::Collections::Vector<DeviceInformationDisplay^>();
        DeviceWatcherHelper^ deviceWatcherHelper;
    };
}
