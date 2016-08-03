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
#include "Scenario3_WriteAndReadTextInAFile.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;

Scenario3::Scenario3() : rootPage(MainPage::Current)
{
    InitializeComponent();
    rootPage->Initialize();
    rootPage->ValidateFile();
    WriteTextButton->Click += ref new RoutedEventHandler(this, &Scenario3::WriteTextButton_Click);
    ReadTextButton->Click  += ref new RoutedEventHandler(this, &Scenario3::ReadTextButton_Click);
}

void Scenario3::WriteTextButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        String^ userContent = InputTextBox->Text;
        if (userContent != nullptr && !userContent->IsEmpty())
        {
            create_task(FileIO::WriteTextAsync(file, userContent)).then([this, file, userContent](task<void> task)
            {
                try
                {
                    task.get();
                    rootPage->NotifyUser("The following text was written to '" + file->Name + "':\n" + userContent, NotifyType::StatusMessage);
                }
                catch (COMException^ ex)
                {
                    rootPage->HandleIoException(ex, "Error writing to '" + file->Name + "'");
                }
            });
        }
        else
        {
            rootPage->NotifyUser("The text box is empty, please write something and then click 'Write' again.", NotifyType::ErrorMessage);
        }
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}

void Scenario3::ReadTextButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        create_task(FileIO::ReadTextAsync(file)).then([this, file](task<String^> task)
        {
            try
            {
                String^ fileContent = task.get();
                rootPage->NotifyUser("The following text was read from '" + file->Name + "':\n" + fileContent, NotifyType::StatusMessage);
            }
            catch(COMException^ ex)
            {
                rootPage->HandleIoException(ex, "Error reading from '" + file->Name + "'");
            }
        });
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}
