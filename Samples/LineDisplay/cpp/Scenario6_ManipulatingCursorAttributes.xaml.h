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

#include "Scenario6_ManipulatingCursorAttributes.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class Scenario6_ManipulatingCursorAttributes sealed
    {
    public:
        Scenario6_ManipulatingCursorAttributes();

        property Windows::Foundation::Collections::IObservableVector<Windows::Devices::PointOfService::LineDisplayCursorType>^ SupportedCursorTypes
        {
            Windows::Foundation::Collections::IObservableVector<Windows::Devices::PointOfService::LineDisplayCursorType>^ get()
            {
                return supportedCursorTypes;
            }
        }

    protected:
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::Devices::PointOfService::ClaimedLineDisplay^ lineDisplay;

        Windows::Foundation::Collections::IObservableVector<Windows::Devices::PointOfService::LineDisplayCursorType>^ supportedCursorTypes =
            ref new Platform::Collections::Vector<Windows::Devices::PointOfService::LineDisplayCursorType>;

        Concurrency::task<void> InitializeAsync();

        void UpdateButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void DisplayTextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ResetButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
