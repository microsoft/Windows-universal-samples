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
#include "Scenario1.h"
#include "Scenario1.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1::Scenario1()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario1::CreateFileButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        StorageFolder storageFolder = co_await KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary);
        try
        {
            SampleState::SampleFile(co_await storageFolder.CreateFileAsync(SampleState::FileName(), CreationCollisionOption::ReplaceExisting));
            rootPage.NotifyUser(L"The file '" + SampleState::SampleFile().Name() + L"' was created.", NotifyType::StatusMessage);
        }
        catch (const hresult_error& ex)
        {
            // I/O errors are reported as exceptions.
            rootPage.NotifyUser(
                L"Error creating file '" + SampleState::FileName() + L"': " + ex.message(), NotifyType::ErrorMessage);
        }
    }
}
