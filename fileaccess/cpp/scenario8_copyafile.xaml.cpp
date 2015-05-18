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
#include "Scenario8_CopyAFile.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;

Scenario8::Scenario8() : rootPage(MainPage::Current)
{
    InitializeComponent();
    rootPage->Initialize();
    rootPage->ValidateFile();
    CopyFileButton->Click += ref new RoutedEventHandler(this, &Scenario8::CopyFileButton_Click);
}

void Scenario8::CopyFileButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        // Get the returned file and copy it
        StorageFolder^ picturesFolder = KnownFolders::PicturesLibrary;
        create_task(file->CopyAsync(picturesFolder, "sample - Copy.dat", NameCollisionOption::ReplaceExisting)).then([this, file](task<StorageFile^> task)
        {
            try
            {
                StorageFile^ sampleFileCopy = task.get();
                rootPage->NotifyUser("The file '" + file->Name + "' was copied and the new file was named '" + sampleFileCopy->Name + "'.", NotifyType::StatusMessage);
            }
            catch (COMException^ ex)
            {
                rootPage->HandleFileNotFoundException(ex);
            }
        });
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}
