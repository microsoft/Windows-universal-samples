// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Media::Playlists;
using namespace Windows::Storage; 
using namespace Windows::Storage::Pickers;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Create a playlist", "SDKTemplate.Scenario1_Create" },
    { "Display a playlist", "SDKTemplate.Scenario2_Display" },
    { "Add items to a playlist", "SDKTemplate.Scenario3_Add" },
    { "Remove an item from a playlist", "SDKTemplate.Scenario4_Remove" },
    { "Clear a playlist", "SDKTemplate.Scenario5_Clear" },
};

Array<String^>^ MainPage::audioExtensionsInner = ref new Array<String^>
{
    L".wma",  L".mp3",  L".mp2",  L".aac", L".adt", L".adts", L".m4a"
};

Array<String^>^ MainPage::playlistExtensionsInner = ref new Array<String^>
{
    L".m3u", L".wpl", L".zpl"
};

FileOpenPicker^ MainPage::CreateFilePicker(Array<String^>^ extensions)
{
    FileOpenPicker^ picker = ref new FileOpenPicker();
    picker->SuggestedStartLocation = PickerLocationId::MusicLibrary;

    // Load the picker with the desired extensions.
    for (unsigned int i = 0; i < extensions->Length; i++)
    {
        picker->FileTypeFilter->Append(extensions[i]);
    }

    return picker;
}

// Returns the playlist the user picked.
// If the user did not pick a playlist, or the playlist could not be loaded,
// then returns nullptr and reports the error to the user.
task<Playlist^> MainPage::PickPlaylistAsync()
{
    // Pick a file, then load it as a playlist.
    FileOpenPicker^ picker = MainPage::CreateFilePicker(MainPage::playlistExtensions);
    return create_task(picker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            return create_task(Playlist::LoadAsync(file)).then([this](task<Playlist^> playlistTask)
            {
                try
                {
                    return playlistTask.get();
                }
                catch (Exception^ ex)
                {
                    NotifyUser(ex->Message, NotifyType::ErrorMessage);
                    return (Playlist^)nullptr;
                }
            });
        }
        else
        {
            NotifyUser("No playlist picked.", NotifyType::ErrorMessage);
            return create_task([]() -> Playlist^ { return nullptr; });
        }
    });
}

// Returns true if the playlist was saved successfully.
// On failure, returns false and reports the error to the user.
task<bool> MainPage::TrySavePlaylistAsync(Playlist^ playlist)
{
    return create_task(playlist->SaveAsync()).then([this](task<void> saveTask)
    {
        try
        {
            saveTask.get();
            return true;
        }
        catch (Exception^ ex)
        {
            NotifyUser(ex->Message, NotifyType::ErrorMessage);
            return false;
        }
    });
}
