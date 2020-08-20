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
#include "Scenario2_UnpinTile.h"
#include "Scenario2_UnpinTile.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::StartScreen;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_UnpinTile::Scenario2_UnpinTile()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario2_UnpinTile::UnpinSecondaryTile_Click(IInspectable sender, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        if (SecondaryTile::Exists(SampleHelpers::LogoSecondaryTileId))
        {
            // First prepare the tile to be unpinned
            SecondaryTile secondaryTile(SampleHelpers::LogoSecondaryTileId);
            // Now make the delete request.
            bool isUnpinned = co_await secondaryTile.RequestDeleteForSelectionAsync(SampleHelpers::GetElementRect(sender), Windows::UI::Popups::Placement::Below);
            if (isUnpinned)
            {
                rootPage.NotifyUser(L"Secondary tile successfully unpinned.", NotifyType::StatusMessage);
            }
            else
            {
                rootPage.NotifyUser(L"Secondary tile not unpinned.", NotifyType::ErrorMessage);
            }
        }
        else
        {
            rootPage.NotifyUser(hstring(SampleHelpers::LogoSecondaryTileId) + L" is not currently pinned.", NotifyType::ErrorMessage);
        }
    }
}
