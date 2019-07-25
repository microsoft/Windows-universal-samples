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
#include "Scenario8.h"
#include "Scenario8.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario8::Scenario8()
    {
        InitializeComponent();
    }

    void Scenario8::OnNavigatedTo(NavigationEventArgs const&)
    {
        SampleState::ValidateFile();
    }

    fire_and_forget Scenario8::CopyFileButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        StorageFile file = SampleState::SampleFile();
        if (file != nullptr)
        {
            try
            {
                StorageFolder picturesLibrary = co_await KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary);
                StorageFile fileCopy = co_await file.CopyAsync(picturesLibrary, L"sample - Copy.dat", NameCollisionOption::ReplaceExisting);
                rootPage.NotifyUser(L"The file '" + file.Name() + L"' was copied and the new file was named '" + fileCopy.Name() + L"'.", NotifyType::StatusMessage);
            }
            catch (const hresult_error& ex)
            {
                // I/O errors are reported as exceptions.
                rootPage.NotifyUser(L"Error copying file '" + file.Name() + L"': " + ex.message(), NotifyType::ErrorMessage);
            }
        }
        else
        {
            SampleState::NotifyUserFileNotExist();
        }
    }
}
