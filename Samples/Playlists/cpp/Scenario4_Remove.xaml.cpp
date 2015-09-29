// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario4_Remove.xaml.h"

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

Scenario4_Remove::Scenario4_Remove() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario4_Remove::PickPlaylistButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Pick a file, load it as a playlist, then remove the last item in the playlist.
    this->rootPage->PickPlaylistAsync().then([this](Playlist^ playlist)
    {
        if (playlist != nullptr)
        {
            if (playlist->Files->Size > 0)
            {
                // Remove the last item.
                playlist->Files->RemoveAtEnd();

                this->rootPage->TrySavePlaylistAsync(playlist).then([this](bool result)
                {
                    if (result)
                    {
                        this->rootPage->NotifyUser("The last item in the playlist was removed.", NotifyType::StatusMessage);
                    }
                });
            }
            else
            {
                this->rootPage->NotifyUser("No items in playlist.", NotifyType::ErrorMessage);
            }
        }
    });
}
