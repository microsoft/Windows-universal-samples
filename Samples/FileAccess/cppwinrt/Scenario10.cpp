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
#include "Scenario10.h"
#include "Scenario10.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario10::Scenario10()
    {
        InitializeComponent();
    }

    void Scenario10::OnNavigatedTo(NavigationEventArgs const&)
    {
        SampleState::ValidateFile();
    }

    fire_and_forget Scenario10::DeleteFileButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        StorageFile file = SampleState::SampleFile();
        if (file != nullptr)
        {
            hstring filename = file.Name();
            try
            {
                co_await file.DeleteAsync();
                SampleState::SampleFile(nullptr);
                rootPage.NotifyUser(L"The file '" + filename + L"' was deleted.", NotifyType::StatusMessage);
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
                    rootPage.NotifyUser(L"Error deleting file '" + filename + L"': " + ex.message(), NotifyType::ErrorMessage);
                }
            }
        }
        else
        {
            SampleState::NotifyUserFileNotExist();
        }
    }
}
