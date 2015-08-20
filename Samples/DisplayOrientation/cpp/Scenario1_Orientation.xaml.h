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

#include "Scenario1_Orientation.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_Orientation sealed
    {
    public:
        Scenario1_Orientation();
        void ApplyRotationLock();

    protected:
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        Windows::Graphics::Display::DisplayInformation^ displayInfo;

        void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ e);
        static Platform::String^ OrientationsToString(Windows::Graphics::Display::DisplayOrientations orientations);
        void UpdateContent();

        Windows::Foundation::EventRegistrationToken orientationChangedToken;
    };
}
