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
    public ref class Scenario5_GetSingleDevice sealed
    {
    public:
        Scenario5_GetSingleDevice();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void GetButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        MainPage^ rootPage = MainPage::Current;
        Windows::Foundation::Collections::IObservableVector<DeviceInformationDisplay^>^ resultCollection = ref new Platform::Collections::Vector<DeviceInformationDisplay^>();
        Windows::Devices::Enumeration::DeviceInformationKind deviceInformationKind = Windows::Devices::Enumeration::DeviceInformationKind::Unknown;
    };
}
