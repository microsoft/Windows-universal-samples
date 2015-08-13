// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario2_Display.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_Display sealed
    {
    public:
        Scenario2_Display();
    private:
        MainPage^ rootPage;
        void PickPlaylistButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
