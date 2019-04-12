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
#include "Scenario4_Bytes.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario4::Scenario4()
{
    InitializeComponent();
}

void Scenario4::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage->ValidateFile();
}

void Scenario4::WriteBytesButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        String^ userContent = InputTextBox->Text;
        IBuffer^ buffer = MainPage::GetBufferFromString(userContent);
        create_task(FileIO::WriteBufferAsync(file, buffer)).then([this, file, buffer, userContent](task<void> task)
        {
            try
            {
                task.get();
                rootPage->NotifyUser("The following " + buffer->Length.ToString() + " bytes of text were written to '" + file->Name + "':\n" + userContent, NotifyType::StatusMessage);
            }
            catch (COMException^ ex)
            {
                // I/O errors are reported as exceptions.
                rootPage->HandleIoException(ex, "Error writing to '" + file->Name + "'");
            }
        });
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}

void Scenario4::ReadBytesButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        create_task(FileIO::ReadBufferAsync(file)).then([this, file](task<IBuffer^> task)
        {
            try
            {
                IBuffer^ buffer = task.get();
                String^ fileContent = MainPage::GetStringFromBuffer(buffer);
                rootPage->NotifyUser("The following " + buffer->Length.ToString() + " bytes of text were read from '" + file->Name + "':\n" + fileContent, NotifyType::StatusMessage);
            }
            catch (COMException^ ex)
            {
                if (ex->HResult == HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION))
                {
                    rootPage->NotifyUser("File is not UTF-8 encoded.", NotifyType::ErrorMessage);
                }
                else
                {
                    // I/O errors are reported as exceptions.
                    rootPage->HandleIoException(ex, "Error reading from '" + file->Name + "'");
                }
            }
        });
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}
