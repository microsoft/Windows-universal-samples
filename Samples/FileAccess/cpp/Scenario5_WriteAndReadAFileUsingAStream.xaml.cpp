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
#include "Scenario5_WriteAndReadAFileUsingAStream.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario5::Scenario5() : rootPage(MainPage::Current)
{
    InitializeComponent();
    rootPage->Initialize();
    rootPage->ValidateFile();
    WriteToStreamButton->Click  += ref new RoutedEventHandler(this, &Scenario5::WriteToStreamButton_Click);
    ReadFromStreamButton->Click += ref new RoutedEventHandler(this, &Scenario5::ReadFromStreamButton_Click);
}

void Scenario5::WriteToStreamButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        String^ userContent = InputTextBox->Text;
        if (userContent != nullptr && !userContent->IsEmpty())
        {
            create_task(file->OpenTransactedWriteAsync()).then([this, file, userContent](task<StorageStreamTransaction^> task)
            {
                try
                {
                    StorageStreamTransaction^ transaction = task.get();
                    auto dataWriter = ref new DataWriter(transaction->Stream);
                    dataWriter->WriteString(userContent);
                    create_task(dataWriter->StoreAsync()).then([this, file, dataWriter, transaction, userContent](unsigned int bytesWritten)
                    {
                        transaction->Stream->Size = bytesWritten; // reset stream size to override the file
                        create_task(transaction->CommitAsync()).then([this, file, transaction, userContent]()
                        {
                            delete transaction; // As a best practice, explicitly close the transaction resource as soon as it is no longer needed.
                            rootPage->NotifyUser("The following text was written to '" + file->Name + "' using a stream:\n" + userContent, NotifyType::StatusMessage);
                        });
                    });
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

void Scenario5::ReadFromStreamButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        create_task(file->OpenAsync(FileAccessMode::Read)).then([this, file](task<IRandomAccessStream^> task)
        {
            try
            {
                IRandomAccessStream^ readStream = task.get();
                UINT64 const size = readStream->Size;
                if (size <= MAXUINT32)
                {
                    auto dataReader = ref new DataReader(readStream);
                    create_task(dataReader->LoadAsync(static_cast<UINT32>(size))).then([this, file, dataReader](unsigned int numBytesLoaded)
                    {
                        String^ fileContent = dataReader->ReadString(numBytesLoaded);
                        delete dataReader; // As a best practice, explicitly close the dataReader resource as soon as it is no longer needed.
                        rootPage->NotifyUser("The following text was read from '" + file->Name + "' using a stream:\n" + fileContent, NotifyType::StatusMessage);
                    });
                }
                else
                {
                    delete readStream; // As a best practice, explicitly close the readStream resource as soon as it is no longer needed.
                    rootPage->NotifyUser("File " + file->Name + " is too big for LoadAsync to load in a single chunk. Files larger than 4GB need to be broken into multiple chunks to be loaded by LoadAsync.", NotifyType::ErrorMessage);
                }
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
