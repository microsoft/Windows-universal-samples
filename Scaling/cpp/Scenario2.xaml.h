//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario2.xaml.h
// Declaration of the Scenario2 class
//

#pragma once

#include "pch.h"
#include "Scenario2.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2 sealed
    {
    public:
        Scenario2();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        // Helpers to convert between points and pixels.
        double PtFromPx(double pixel);
        double PxFromPt(double pt);

        Platform::String^ StringFromDouble(double);
        void OutputSettings(double rawPixelsPerViewPixel, Windows::UI::Xaml::FrameworkElement^ rectangle, Windows::UI::Xaml::Controls::TextBlock^ relativePxText,
                            Windows::UI::Xaml::Controls::TextBlock^ physicalPxText, Windows::UI::Xaml::Controls::TextBlock^ fontTextBlock);
        void ResetOutput();
        void DisplayProperties_DpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);

        Windows::Foundation::EventRegistrationToken token;
    };
}
