// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario4_Remove.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario4_Remove sealed
    {
    public:
        Scenario4_Remove();
    private:
        MainPage^ rootPage;
        void PickPlaylistButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
