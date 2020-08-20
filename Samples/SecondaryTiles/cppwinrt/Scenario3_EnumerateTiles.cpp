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
#include "Scenario3_EnumerateTiles.h"
#include "Scenario3_EnumerateTiles.g.cpp"
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::StartScreen;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_EnumerateTiles::Scenario3_EnumerateTiles()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario3_EnumerateTiles::EnumerateSecondaryTiles_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Get secondary tile ids for this package
        IVectorView<SecondaryTile> tilelist = co_await SecondaryTile::FindAllAsync();
        if (tilelist.Size() > 0)
        {
            std::wostringstream output;
            for (SecondaryTile tile : tilelist)
            {
                output << L"Tile \"" << std::wstring_view{ tile.TileId() } <<
                    L"\", Display name = \"" << std::wstring_view{ tile.DisplayName() } << L"\"" << std::endl;
            }
            rootPage.NotifyUser(output.str(), NotifyType::StatusMessage);
        }
        else
        {
            rootPage.NotifyUser(L"No secondary tiles are available for this appId.", NotifyType::ErrorMessage);
        }
    }
}
