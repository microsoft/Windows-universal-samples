// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario2_Display.xaml.h"
#include <sstream>

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Playlists;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_Display::Scenario2_Display() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario2_Display::PickPlaylistButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    this->rootPage->PickPlaylistAsync().then([this](Playlist^ playlist)
    {
        if (playlist != nullptr)
        {
            // create a vector to store the async operations
            std::vector<task<MusicProperties^>> results(playlist->Files->Size);
            for (unsigned int i = 0; i < playlist->Files->Size; i++)
            {
                StorageFile^ file = playlist->Files->GetAt(i);
                results[i] = create_task(file->Properties->GetMusicPropertiesAsync());
            }

            // wait for all of the async operations to complete and then output the results
            when_all(results.begin(), results.end()).then([this, playlist](std::vector<MusicProperties^> results)
            {
                String^ result = "Songs in playlist: " + playlist->Files->Size.ToString() + "\n";
                for (unsigned int i = 0; i < results.size(); i++)
                {
                    MusicProperties^ properties = results[i];
                    result += "\n";
                    result += "Path: " + playlist->Files->GetAt(i)->Path + "\n";
                    result += "Title: " + properties->Title + "\n";
                    result += "Album: " + properties->Album + "\n";
                    result += "Artist: " + properties->Artist + "\n";
                }
                this->OutputStatus->Text = result;
            });
        }
    });
}
