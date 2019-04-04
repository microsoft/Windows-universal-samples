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
#include "Scenario5_Stream.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario5::Scenario5()
{
    InitializeComponent();
}

void Scenario5::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage->ValidateFile();
}

void Scenario5::WriteToStreamButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        String^ userContent = InputTextBox->Text;
        create_task(file->OpenTransactedWriteAsync()).then([this, userContent](StorageStreamTransaction^ transaction)
        {
            IBuffer^ buffer = MainPage::GetBufferFromString(userContent);
            return create_task(transaction->Stream->WriteAsync(buffer)).then([transaction](unsigned int bytesWritten)
            {
                transaction->Stream->Size = bytesWritten; // truncate file
                return create_task(transaction->CommitAsync());
            }).then([transaction]()
            {
                delete transaction; // As a best practice, explicitly close the transaction resource as soon as it is no longer needed.
            });
        }).then([this, file, userContent]()
        {
            rootPage->NotifyUser("The following text was written to '" + file->Name + "' using a stream:\n" + userContent, NotifyType::StatusMessage);
        }).then([this, file](task<void> previousTask)
        {
            try
            {
                previousTask.get();
            }
            catch (COMException^ ex)
            {
                rootPage->HandleIoException(ex, "Error writing to '" + file->Name + "'");
            }
        });
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}

void Scenario5::ReadFromStreamButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        create_task(file->OpenAsync(FileAccessMode::Read)).then([this, file](IRandomAccessStream^ readStream)
        {
            UINT64 size64 = readStream->Size;
            if (size64 <= MAXUINT32)
            {
                UINT32 size32 = static_cast<UINT32>(size64);
                IBuffer^ buffer = ref new Buffer(size32);
                return create_task(readStream->ReadAsync(buffer, size32, InputStreamOptions::None)).then([this, file, readStream](IBuffer^ buffer)
                {
                    delete readStream; // As a best practice, explicitly close the readStream resource as soon as it is no longer needed.
                    String^ fileContent = MainPage::GetStringFromBuffer(buffer);
                    rootPage->NotifyUser("The following text was read from '" + file->Name + "' using a stream:\n" + fileContent, NotifyType::StatusMessage);
                });
            }
            else
            {
                delete readStream; // As a best practice, explicitly close the readStream resource as soon as it is no longer needed.
                rootPage->NotifyUser("File " + file->Name + " is too big for LoadAsync to load in a single chunk. Files larger than 4GB need to be broken into multiple chunks to be loaded by LoadAsync.", NotifyType::ErrorMessage);
                return task_from_result();
            }
        }).then([this, file](task<void> previousTask)
        {
            try
            {
                previousTask.get();
            }
            catch (COMException^ ex)
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
