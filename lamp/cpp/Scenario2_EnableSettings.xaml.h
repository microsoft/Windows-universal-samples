//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include "Scenario2_EnableSettings.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_EnableSettings sealed
    {
    public:
        Scenario2_EnableSettings();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ _rootPage;

        void BrightnessBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ColorLampBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        Concurrency::task<void> LogStatusToOutputBoxAsync(Platform::String^ message);
        Concurrency::task<void> LogStatusAsync(Platform::String^ message, NotifyType type);
    };
}
