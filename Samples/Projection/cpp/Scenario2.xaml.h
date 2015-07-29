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

#include "Scenario2.g.h"
#include "MainPage.xaml.h"

namespace SDKSample
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2 sealed
    {
    public:
        Scenario2();
    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    private:
        int thisViewId;
        void StartProjecting_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void RequestAndStartProjecting_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnProjectionDisplayAvailableChanged(Platform::Object^ sender, Platform::Object^ e);
        void UpdateTextBlock(bool screenAvailable);
        Windows::Foundation::EventRegistrationToken displayChangeToken;
        Windows::UI::Core::CoreDispatcher^ dispatcher;
    };
}