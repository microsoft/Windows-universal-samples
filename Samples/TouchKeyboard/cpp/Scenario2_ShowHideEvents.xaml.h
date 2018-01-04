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

#include "Scenario2_ShowHideEvents.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// Sample page to subscribe show/hide event of touch keyboard.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_ShowHideEvents sealed
    {
    public:
        Scenario2_ShowHideEvents();

    protected:
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void OnHiding(Windows::UI::ViewManagement::InputPane^ sender, Windows::UI::ViewManagement::InputPaneVisibilityEventArgs^ args);
        void OnShowing(Windows::UI::ViewManagement::InputPane^ sender, Windows::UI::ViewManagement::InputPaneVisibilityEventArgs^ args);

        Windows::Foundation::EventRegistrationToken hidingEventToken;
        Windows::Foundation::EventRegistrationToken showingEventToken;
    };
}
