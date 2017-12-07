// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario3_BackgroundDeviceWatcher.g.h"
#include "MainPage.xaml.h"
#include "DisplayHelpers.h"

// Needed for the DeviceWatcherTrigger
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Storage;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3 sealed
    {
    public:
        Scenario3();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void StartWatcher();
        void StopWatcher();

        void RegisterBackgroundTask(DeviceWatcherTrigger^ deviceWatcherTrigger);
        void UnregisterBackgroundTask(IBackgroundTaskRegistration^ taskReg);

        SDKTemplate::MainPage^ rootPage;
        IBackgroundTaskRegistration^ backgroundTaskRegistration;
        Platform::String^ backgroundTaskName = "DeviceEnumerationCpp_BackgroundTaskName";
    };
}
