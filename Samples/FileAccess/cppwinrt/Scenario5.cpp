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
#include "Scenario5.h"
#include "Scenario5.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario5::Scenario5()
    {
        InitializeComponent();
    }

    void Scenario5::OnNavigatedTo(NavigationEventArgs const&)
    {
        SampleState::ValidateFile();
    }

    fire_and_forget Scenario5::WriteToStreamButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        StorageFile file = SampleState::SampleFile();
        if (file != nullptr)
        {
            try
            {
                hstring userContent = InputTextBox().Text();
                StorageStreamTransaction transaction = co_await file.OpenTransactedWriteAsync();
                IBuffer buffer = SampleState::GetBufferFromString(userContent);
                uint32_t bytesWritten = co_await transaction.Stream().WriteAsync(buffer);
                transaction.Stream().Size(bytesWritten); // truncate any previous data in the file
                co_await transaction.CommitAsync();
                transaction.Close(); // As a best practice, explicitly close the transaction resource as soon as it is no longer needed.
                rootPage.NotifyUser(L"The following text was written to '" + file.Name() + L"' using a stream:\n" + userContent, NotifyType::StatusMessage);
            }
            catch (const hresult_error& ex)
            {
                if (ex.code() == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                {
                    SampleState::NotifyUserFileNotExist();
                }
                else
                {
                    // I/O errors are reported as exceptions.
                    rootPage.NotifyUser(L"Error writing to '" + file.Name() + L"': " + ex.message(), NotifyType::ErrorMessage);
                }
            }
        }
        else
        {
            SampleState::NotifyUserFileNotExist();
        }
    }

    fire_and_forget Scenario5::ReadFromStreamButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        StorageFile file = SampleState::SampleFile();
        if (file != nullptr)
        {
            try
            {
                IRandomAccessStream readStream = co_await file.OpenAsync(FileAccessMode::Read);
                uint64_t size64 = readStream.Size();
                if (size64 <= std::numeric_limits<uint32_t>::max())
                {
                    uint32_t size32 = static_cast<uint32_t>(size64);
                    IBuffer buffer = Buffer{ size32 };
                    buffer = co_await readStream.ReadAsync(buffer, size32, InputStreamOptions::None);
                    hstring fileContent = SampleState::GetStringFromBuffer(buffer);
                    rootPage.NotifyUser(L"The following text was read from '" + file.Name() + L"' using a stream:\n" + fileContent, NotifyType::StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser(L"File '" + file.Name() + L"' is too big to load in a single buffer. Files larger than 4GB need to be broken into multiple buffers to be read by ReadAsync.", NotifyType::ErrorMessage);
                }
            }
            catch (const hresult_error& ex)
            {
                if (ex.code() == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                {
                    SampleState::NotifyUserFileNotExist();
                }
                else if (ex.code() == HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION))
                {
                    rootPage.NotifyUser(L"File is not UTF-8 encoded.", NotifyType::ErrorMessage);
                }
                else
                {
                    // I/O errors are reported as exceptions.
                    rootPage.NotifyUser(L"Error reading from '" + file.Name() + L"': " + ex.message(), NotifyType::ErrorMessage);
                }
            }
        }
        else
        {
            SampleState::NotifyUserFileNotExist();
        }
    }
}
