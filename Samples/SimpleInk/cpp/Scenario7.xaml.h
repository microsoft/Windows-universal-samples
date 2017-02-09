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

#include "Scenario7.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario7 sealed
    {
    public:
        Scenario7();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void InkToolbar_IsRulerButtonCheckedChanged(Windows::UI::Xaml::Controls::InkToolbar^ sender, Platform::Object^ args);
        void OnBringIntoView(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        MainPage^ rootPage = MainPage::Current;
        Windows::UI::Input::Inking::InkPresenterRuler^ ruler;
    };
}
