// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_DevicePicker.g.h"
#include "MainPage.xaml.h"
#include "DisplayHelpers.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class Scenario1 sealed
    {
    public:
        Scenario1();

        property Windows::Foundation::Collections::IObservableVector<DeviceInformationDisplay^>^ ResultCollection;

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void PickSingleDeviceButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ShowDevicePickerButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void ShowDevicePicker(bool pickSingle);

        SDKTemplate::MainPage^ rootPage;
        Windows::Devices::Enumeration::DevicePicker^ devicePicker;
    };
}
