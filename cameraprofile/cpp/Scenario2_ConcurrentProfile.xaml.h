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

#include "Scenario2_ConcurrentProfile.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_ConcurrentProfile sealed
    {
    public:
        Scenario2_ConcurrentProfile();

        void CheckConcurrentProfileBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ _rootPage;
        Concurrency::task<Platform::String^> GetVideoProfileSupportedDeviceIdAsync(Windows::Devices::Enumeration::Panel panel);

        Concurrency::task<void> LogStatusToOutputBoxAsync(Platform::String^ message);
        Concurrency::task<void> LogStatusAsync(Platform::String^ message, NotifyType type);
    };
}
