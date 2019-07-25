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
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1::Scenario1()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario1::GetFilesAndFoldersButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        StorageFolder picturesFolder = co_await KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary);
        IVectorView<StorageFile> fileList = co_await picturesFolder.GetFilesAsync();
        IVectorView<StorageFolder> folderList = co_await picturesFolder.GetFoldersAsync();

        uint32_t count = fileList.Size() + folderList.Size();

        std::wstringstream output;
        output << std::wstring_view{ picturesFolder.Name() } << L" (" << count << L")\n\n";

        for (StorageFolder const& folder : folderList)
        {
            output << L"    " << std::wstring_view{ folder.DisplayName() } << L"\\" << std::endl;
        }

        for (StorageFile const& file : fileList)
        {
            output << L"    " << std::wstring{ file.Name() } << std::endl;
        }

        OutputTextBlock().Text(output.str());
    }
}
