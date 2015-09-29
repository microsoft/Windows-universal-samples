// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario3_Add.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_Add sealed
    {
    public:
        Scenario3_Add();
    private:
        MainPage^ rootPage;
        Windows::Media::Playlists::Playlist^ playlist;
        void PickPlaylistButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PickAudioButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
