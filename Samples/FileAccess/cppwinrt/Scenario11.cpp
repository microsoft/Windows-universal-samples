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
#include "Scenario11.h"
#include "Scenario11.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario11::Scenario11()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario11::GetFileButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        StorageFolder storageFolder = co_await KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary);
        StorageFile file = (co_await storageFolder.TryGetItemAsync(SampleState::FileName())).try_as<StorageFile>();
        if (file != nullptr)
        {
            rootPage.NotifyUser(L"Operation result: " + file.Name(), NotifyType::StatusMessage);
        }
        else
        {
            rootPage.NotifyUser(L"Operation result: null", NotifyType::StatusMessage);
        }
    }
}
