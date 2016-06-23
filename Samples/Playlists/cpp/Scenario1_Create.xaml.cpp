// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_Create.xaml.h"
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

Scenario1_Create::Scenario1_Create() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario1_Create::PickAudioButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Pick multiple files, then create the playlist.
    FileOpenPicker^ picker = MainPage::CreateFilePicker(MainPage::audioExtensions);

    create_task(picker->PickMultipleFilesAsync()).then([this](IVectorView<StorageFile^>^ files)
    {
        if (files->Size > 0)
        {
            Playlist^ playlist = ref new Playlist();

            for (StorageFile^ file : files)
            {
                playlist->Files->Append(file);
            }

            StorageFolder^ folder = KnownFolders::MusicLibrary;
            String^ name = "Sample";
            NameCollisionOption collisionOption = NameCollisionOption::ReplaceExisting;
            PlaylistFormat format = PlaylistFormat::WindowsMedia;

            create_task(playlist->SaveAsAsync(folder, name, collisionOption, format)).then([this, files](task<StorageFile^> fileTask)
            {
                try
                {
                    StorageFile^ file = fileTask.get();
                    this->rootPage->NotifyUser("The playlist " + file->Name + " was created and saved with " + files->Size.ToString() + " files.", NotifyType::StatusMessage);
                }
                catch (Exception^ ex)
                {
                    this->rootPage->NotifyUser(ex->Message, NotifyType::ErrorMessage);
                }
            });
        }
        else
        {
            rootPage->NotifyUser("No files picked.", NotifyType::ErrorMessage);
        }
    });
}
