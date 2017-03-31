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
#include "Scenario11_TryToGetAFileWithoutGettingAnError.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;

Scenario11::Scenario11() : rootPage(MainPage::Current)
{
    InitializeComponent();
    rootPage->Initialize();
    GetFileButton->Click += ref new RoutedEventHandler(this, &Scenario11::GetFileButton_Click);
}

void Scenario11::GetFileButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    create_task(KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary)).then([](StorageFolder^ picturesFolder)
    {
        // Gets a file without throwing an exception
        return picturesFolder->TryGetItemAsync("sample.dat");
    }).then([this](IStorageItem^ item)
    {
        if (item != nullptr)
        {
            rootPage->NotifyUser("Operation result: " + item->Name, NotifyType::StatusMessage);
        }
        else
        {
            rootPage->NotifyUser("Operation result: null", NotifyType::StatusMessage);
        }
    });
}
