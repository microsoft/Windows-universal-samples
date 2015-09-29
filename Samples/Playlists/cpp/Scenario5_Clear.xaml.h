// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario5_Clear.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// Display animation metrics.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario5_Clear sealed
    {
    public:
        Scenario5_Clear();
    private:
        MainPage^ rootPage;
        void PickPlaylistButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
