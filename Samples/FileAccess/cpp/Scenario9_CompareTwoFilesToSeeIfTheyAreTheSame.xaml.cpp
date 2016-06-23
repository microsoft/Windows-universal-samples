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
#include "Scenario9_CompareTwoFilesToSeeIfTheyAreTheSame.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;

Scenario9::Scenario9() : rootPage(MainPage::Current)
{
    InitializeComponent();
    rootPage->Initialize();
    rootPage->ValidateFile();
    CompareFilesButton->Click += ref new RoutedEventHandler(this, &Scenario9::CompareFilesButton_Click);
}

void Scenario9::CompareFilesButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        // Compares a picked file with sample.dat
        FileOpenPicker^ picker = ref new FileOpenPicker();
        picker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
        picker->FileTypeFilter->Append("*");
        create_task(picker->PickSingleFileAsync()).then([this, file](StorageFile^ comparand)
        {
            if (comparand != nullptr)
            {
                try
                {
                    if (file->IsEqual(comparand))
                    {
                        rootPage->NotifyUser("Files are equal", NotifyType::StatusMessage);
                    }
                    else
                    {
                        rootPage->NotifyUser("Files are not equal", NotifyType::StatusMessage);
                    }
                }
                catch (COMException^ ex)
                {
                    rootPage->HandleIoException(ex, "Error determining whether two files are the same");
                }
            }
            else
            {
                rootPage->NotifyUser("Operation cancelled", NotifyType::StatusMessage);
            }
        });
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}
