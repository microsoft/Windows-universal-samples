// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario2_DeviceWatcher.g.h"
#include "MainPage.xaml.h"
#include "DisplayHelpers.h"
#include "DeviceWatcherHelper.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_DeviceWatcher sealed
    {
    public:
        Scenario2_DeviceWatcher();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void StartWatcher();
        void StopWatcher();

        MainPage^ rootPage = MainPage::Current;
        Windows::Foundation::Collections::IObservableVector<DeviceInformationDisplay^>^ resultCollection = ref new Platform::Collections::Vector<DeviceInformationDisplay^>();
        DeviceWatcherHelper^ deviceWatcherHelper;
    };
}
