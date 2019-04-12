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
#include "Scenario8_Copy.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

Scenario8::Scenario8()
{
    InitializeComponent();
}

void Scenario8::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage->ValidateFile();
}

void Scenario8::CopyFileButton_Click(Object^ sender, RoutedEventArgs^ e)
{    
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        // Get the returned file and copy it
        create_task(KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary)).then([this, file](StorageFolder^ picturesFolder)
        {
            return file->CopyAsync(picturesFolder, "sample - Copy.dat", NameCollisionOption::ReplaceExisting);
        }).then([this, file](task<StorageFile^> task)
        {
            try
            {
                StorageFile^ sampleFileCopy = task.get();
                rootPage->NotifyUser("The file '" + file->Name + "' was copied and the new file was named '" + sampleFileCopy->Name + "'.", NotifyType::StatusMessage);
            }
            catch (COMException^ ex)
            {
                // I/O errors are reported as exceptions.
                rootPage->HandleIoException(ex, "Error copying file '" + file->Name + "'");
            }
        });
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}
