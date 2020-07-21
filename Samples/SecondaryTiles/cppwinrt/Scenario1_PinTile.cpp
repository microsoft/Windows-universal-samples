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
#include "Scenario1_PinTile.h"
#include "Scenario1_PinTile.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::StartScreen;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_PinTile::Scenario1_PinTile()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario1_PinTile::PinButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Prepare package images for the tile sizes in our tile to be pinned as well as for the square30x30 logo used in the Apps view.  
        Uri square150x150Logo(L"ms-appx:///Assets/square150x150Tile-sdk.png");
        Uri wide310x150Logo(L"ms-appx:///Assets/wide310x150Tile-sdk.png");
        Uri square310x310Logo(L"ms-appx:///Assets/square310x310Tile-sdk.png");

        // During creation of secondary tile, an application may set additional arguments on the tile that will be passed in during activation,
        // so that the app knows which tile the user is launching. In this sample, we'll pass in the date and time the secondary tile was pinned.
        hstring tileActivationArguments = hstring(SampleHelpers::LogoSecondaryTileId) + L" WasPinnedAt=" + SampleHelpers::CurrentTimeAsString();
            
        // Create a Secondary tile with all the required arguments.
        // Note the last argument specifies what size the Secondary tile should show up as by default in the Pin to start fly out.
        // It can be set to TileSize.Square150x150, TileSize.Wide310x150, or TileSize.Default.  
        // If set to TileSize.Wide310x150, then the asset for the wide size must be supplied as well.
        // TileSize.Default will default to the wide size if a wide size is provided, and to the medium size otherwise. 
        SecondaryTile secondaryTile(SampleHelpers::LogoSecondaryTileId,
            L"Title text shown on the tile",
            tileActivationArguments,
            square150x150Logo,
            TileSize::Square150x150);

        secondaryTile.VisualElements().Wide310x150Logo(wide310x150Logo);
        secondaryTile.VisualElements().Square310x310Logo(square310x310Logo);

        // The display of the secondary tile name can be controlled for each tile size.
        // The default is false.
        secondaryTile.VisualElements().ShowNameOnSquare150x150Logo(true);
        secondaryTile.VisualElements().ShowNameOnWide310x150Logo(true);
        secondaryTile.VisualElements().ShowNameOnSquare310x310Logo(true);

        // Specify a foreground text value.
        // The tile background color is inherited from the parent unless a separate value is specified.
        secondaryTile.VisualElements().ForegroundText(ForegroundText::Dark);

        // Set this to false if roaming doesn't make sense for the secondary tile.
        // The default is true.
        secondaryTile.RoamingEnabled(false);

        // The tile is created and we can now attempt to pin the tile.
        // Note that the status message is updated when the async operation to pin the tile completes.
        bool isPinned = co_await secondaryTile.RequestCreateForSelectionAsync(SampleHelpers::GetElementRect(sender), Windows::UI::Popups::Placement::Below);

        if (isPinned)
        {
            rootPage.NotifyUser(L"Secondary tile successfully pinned.", NotifyType::StatusMessage);
        }
        else
        {
            rootPage.NotifyUser(L"Secondary tile not pinned.", NotifyType::ErrorMessage);
        }
    }
}
