// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

//
// Scenario2Input.xaml.h
// Declaration of the Scenario2Input class.
//

#pragma once

#include "pch.h"
#include "ScenarioInput2.g.h"
#include "MainPage.g.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class ScenarioInput2 sealed
    {
    public:
        ScenarioInput2();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        ~ScenarioInput2();
        MainPage^ rootPage;
        void rootPage_OutputFrameLoaded(Object^ sender, Object^ e);
        void ApplyFont_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        Windows::Foundation::EventRegistrationToken _frameLoadedToken;
        FontInfoLocal^ _oriHeadingUI;
        FontInfoLocal^ _oriTextUI;
    };
}
