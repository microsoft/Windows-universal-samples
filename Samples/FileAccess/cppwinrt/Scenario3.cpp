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
#include "Scenario3.h"
#include "Scenario3.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario3::Scenario3()
    {
        InitializeComponent();
    }

    void Scenario3::OnNavigatedTo(NavigationEventArgs const&)
    {
        SampleState::ValidateFile();
    }

    fire_and_forget Scenario3::WriteTextButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        StorageFile file = SampleState::SampleFile();
        if (file != nullptr)
        {
            try
            {
                hstring userContent = InputTextBox().Text();
                co_await FileIO::WriteTextAsync(file, userContent);
                rootPage.NotifyUser(L"The following text was written to '" + file.Name() + L"':\n" + userContent, NotifyType::StatusMessage);
            }
            catch (const hresult_error& ex)
            {
                if (ex.code() == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                {
                    SampleState::NotifyUserFileNotExist();
                }
                else if (ex.code() == HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION))
                {
                    rootPage.NotifyUser(L"File cannot be decoded as Unicode.", NotifyType::ErrorMessage);
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

    fire_and_forget Scenario3::ReadTextButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        StorageFile file = SampleState::SampleFile();
        if (file != nullptr)
        {
            try
            {
                hstring fileContent = co_await FileIO::ReadTextAsync(file);
                    rootPage.NotifyUser(L"The following text was read from '" + file.Name() + L"':\n" + fileContent, NotifyType::StatusMessage);
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
