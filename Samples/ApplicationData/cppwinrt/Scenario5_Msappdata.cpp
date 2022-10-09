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
#include "Scenario5_Msappdata.h"
#include "Scenario5_Msappdata.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario5_Msappdata::Scenario5_Msappdata()
    {
        InitializeComponent();
    }

    IAsyncAction Scenario5_Msappdata::CopyAssetToFolderIfNecessaryAsync(hstring name, StorageFolder const& folder)
    {
        if (co_await folder.TryGetItemAsync(name) == nullptr)
        {
            auto sourceFile = co_await StorageFile::GetFileFromApplicationUriAsync(Uri{ L"ms-appx:///assets/" + name });
            co_await sourceFile.CopyAsync(folder);
        }
    }

    fire_and_forget Scenario5_Msappdata::OnNavigatedTo(NavigationEventArgs const&)
    {
        auto strong = get_strong();
        ApplicationData appData = ApplicationData::Current();

        co_await CopyAssetToFolderIfNecessaryAsync(L"appDataLocal.png", appData.LocalFolder());
        co_await CopyAssetToFolderIfNecessaryAsync(L"appDataRoaming.png", appData.RoamingFolder());
        co_await CopyAssetToFolderIfNecessaryAsync(L"appDataTemp.png", appData.TemporaryFolder());

        LocalImage().Source(BitmapImage{ Uri{L"ms-appdata:///local/appDataLocal.png"} });
        RoamingImage().Source(BitmapImage{ Uri{L"ms-appdata:///roaming/appDataRoaming.png"} });
        TempImage().Source(BitmapImage{ Uri{L"ms-appdata:///temp/appDataTemp.png"} });
    }

    void Scenario5_Msappdata::OnNavigatingFrom(NavigatingCancelEventArgs const&)
    {
        LocalImage().Source(nullptr);
        RoamingImage().Source(nullptr);
        TempImage().Source(nullptr);
    }

}

