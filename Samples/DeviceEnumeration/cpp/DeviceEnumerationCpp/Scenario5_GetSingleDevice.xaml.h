// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario5_GetSingleDevice.g.h"
#include "MainPage.xaml.h"
#include "DisplayHelpers.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class Scenario5 sealed
    {
    public:
        Scenario5();

        property Windows::Foundation::Collections::IObservableVector<DeviceInformationDisplay^>^ ResultCollection;

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void GetButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void InterfaceIdTextBox_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        SDKTemplate::MainPage^ rootPage;
        Windows::Devices::Enumeration::DeviceInformationKind deviceInformationKind;
    };
}
