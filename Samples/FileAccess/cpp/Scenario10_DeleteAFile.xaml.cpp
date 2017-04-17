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
#include "Scenario10_DeleteAFile.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;

Scenario10::Scenario10() : rootPage(MainPage::Current)
{
    InitializeComponent();
    rootPage->Initialize();
    rootPage->ValidateFile();
    DeleteFileButton->Click += ref new RoutedEventHandler(this, &Scenario10::DeleteFileButton_Click);
}

void Scenario10::DeleteFileButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        String^ fileName = file->Name;
        // Deletes the file
        create_task(file->DeleteAsync()).then([this, fileName](task<void> task)
        {
            try
            {
                task.get();
                rootPage->SampleFile = nullptr;
                rootPage->NotifyUser("The file '" + fileName + "' was deleted", NotifyType::StatusMessage);
            }
            catch (COMException^ ex)
            {
                rootPage->HandleIoException(ex, "Error deleting file '" + fileName + "'");
            }
        });
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}
