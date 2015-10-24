// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario5_Clear.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Playlists;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario5_Clear::Scenario5_Clear() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario5_Clear::PickPlaylistButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    this->rootPage->PickPlaylistAsync().then([this](Playlist^ playlist)
    {
        if (playlist != nullptr)
        {
            // Clear playlist.
            playlist->Files->Clear();

            this->rootPage->TrySavePlaylistAsync(playlist).then([this](bool result)
            {
                if (result)
                {
                    this->rootPage->NotifyUser("Playlist cleared.", NotifyType::StatusMessage);
                }
            });
        }
    });
}
