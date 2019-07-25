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
#include "Scenario4.h"
#include "Scenario4.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario4::Scenario4()
    {
        InitializeComponent();
    }

    void Scenario4::OnNavigatedTo(NavigationEventArgs const&)
    {
        SampleState::ValidateFile();
    }

    fire_and_forget Scenario4::WriteBytesButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        StorageFile file = SampleState::SampleFile();
        if (file != nullptr)
        {
            try
            {
                hstring userContent = InputTextBox().Text();
                IBuffer buffer = SampleState::GetBufferFromString(userContent);
                co_await FileIO::WriteBufferAsync(file, buffer);
                rootPage.NotifyUser(L"The following " + to_hstring(buffer.Length()) + L" bytes were written to '" + file.Name() + L"':\n" + userContent, NotifyType::StatusMessage);
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
                    rootPage.NotifyUser(L"Error writing to '" + file.Name() + L"': " + ex.message(), NotifyType::ErrorMessage);
                }
            }
        }
        else
        {
            SampleState::NotifyUserFileNotExist();
        }
    }

    fire_and_forget Scenario4::ReadBytesButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        StorageFile file = SampleState::SampleFile();
        if (file != nullptr)
        {
            try
            {
                IBuffer buffer = co_await FileIO::ReadBufferAsync(file);
                hstring fileContent = SampleState::GetStringFromBuffer(buffer);
                rootPage.NotifyUser(L"The following "+ to_hstring(buffer.Length()) + L"bytes were read from '" + file.Name() + L"':\n" + fileContent, NotifyType::StatusMessage);
            }
            catch (const hresult_error& ex)
            {
                if (ex.code() == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                {
                    SampleState::NotifyUserFileNotExist();
                }
                else if (ex.code() == HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION))
                {
                    // Thrown by CryptographicBuffer::ConvertBinaryToString
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
