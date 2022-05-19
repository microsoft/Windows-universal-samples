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

#pragma once

#include "Scenario5_Msappdata.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario5_Msappdata : Scenario5_MsappdataT<Scenario5_Msappdata>
    {
        Scenario5_Msappdata();

        fire_and_forget OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs const&);

    private:
        Windows::Foundation::IAsyncAction CopyAssetToFolderIfNecessaryAsync(hstring name, Windows::Storage::StorageFolder const& folder);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario5_Msappdata : Scenario5_MsappdataT<Scenario5_Msappdata, implementation::Scenario5_Msappdata>
    {
    };
}

