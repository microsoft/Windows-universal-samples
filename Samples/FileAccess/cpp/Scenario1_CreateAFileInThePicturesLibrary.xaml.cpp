//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "Scenario1_CreateAFileInThePicturesLibrary.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;

Scenario1::Scenario1() : rootPage(MainPage::Current)
{
    InitializeComponent();
    rootPage->Initialize();
    CreateFileButton->Click += ref new RoutedEventHandler(this, &Scenario1::CreateFileButton_Click);
}

void Scenario1::CreateFileButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    create_task(KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary))
        .then([this](StorageFolder^ picturesFolder)
    {
        return picturesFolder->CreateFileAsync(rootPage->Filename, CreationCollisionOption::ReplaceExisting);
    }).then([this](task<StorageFile^> task)
    {
        try
        {
            StorageFile^ file = task.get();
            rootPage->SampleFile = file;
            rootPage->NotifyUser("The file '" + file->Name + "' was created.", NotifyType::StatusMessage);
        }
        catch (Exception^ e)
        {
            // I/O errors are reported as exceptions.
            rootPage->NotifyUser("Error creating file '" + MainPage::Filename + "': " + e->Message, NotifyType::ErrorMessage);
        }
    });
}
