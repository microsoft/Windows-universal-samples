// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario3_Add.xaml.h"
#include <sstream>

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

Scenario3_Add::Scenario3_Add() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario3_Add::PickPlaylistButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    PickAudioButton->IsEnabled = false;
    this->rootPage->PickPlaylistAsync().then([this](Playlist^ playlist)
    {
        this->playlist = playlist;
        if (this->playlist != nullptr)
        {
            this->rootPage->NotifyUser("Playlist loaded", NotifyType::StatusMessage);
        }
        
        PickAudioButton->IsEnabled = (this->playlist != nullptr);
    });
}

void Scenario3_Add::PickAudioButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Pick some files, then add them to the playlist.
    FileOpenPicker^ picker = MainPage::CreateFilePicker(MainPage::audioExtensions);
    create_task(picker->PickMultipleFilesAsync()).then([this](IVectorView<StorageFile^>^ files)
    {
        if (files->Size > 0)
        {
            for (StorageFile^ file : files)
            {
                this->playlist->Files->Append(file);
            }

            this->rootPage->TrySavePlaylistAsync(playlist).then([this, files](bool result)
            {
                if (result)
                {
                    this->rootPage->NotifyUser(files->Size + " files added to the playlist.", NotifyType::StatusMessage);
                }
            });
        }
        else
        {
            this->rootPage->NotifyUser("No file was selected.", NotifyType::ErrorMessage);
        }
    });
}
