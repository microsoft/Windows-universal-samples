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
#include "Scenario7_PinFromAppbar.h"
#include "Scenario7_PinFromAppbar.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::StartScreen;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario7_PinFromAppbar::Scenario7_PinFromAppbar()
    {
        InitializeComponent();
    }

    void Scenario7_PinFromAppbar::OnNavigatedTo(NavigationEventArgs const&)
    {
        // The appbar is open by default.
        Message().Text(L"Tap or click the Pin button to pin a tile.");
        UpdateAppBarButton();

        windowActivatedEventToken = Window::Current().Activated({ get_weak(), &Scenario7_PinFromAppbar::Window_Activated });
    }

    void Scenario7_PinFromAppbar::OnNavigatedFrom(NavigationEventArgs const&e)
    {
        Window::Current().Activated(windowActivatedEventToken);
    }

    void Scenario7_PinFromAppbar::Window_Activated(IInspectable const&, WindowActivatedEventArgs const&)
    {
        // When the window regains activation, refresh the pin/unpin button.
        UpdateAppBarButton();
    }

    // This updates the Pin/Unpin button in the app bar based on whether the
    // secondary tile is already pinned.
    void Scenario7_PinFromAppbar::UpdateAppBarButton()
    {
        if (SecondaryTile::Exists(SampleHelpers::AppBarTileId))
        {
            PinUnPinCommandButton().Label(L"Unpin");
            PinUnPinCommandButton().Icon(SymbolIcon(Symbol::UnPin));
        }
        else
        {
            PinUnPinCommandButton().Label(L"Pin");
            PinUnPinCommandButton().Icon(SymbolIcon(Symbol::Pin));
        }

        PinUnPinCommandButton().UpdateLayout();
    }


    fire_and_forget Scenario7_PinFromAppbar::PinToAppBar_Click(IInspectable sender, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        SecondaryTileCommandBar().IsSticky(true);

        // Let us first verify if we need to pin or unpin
        if (SecondaryTile::Exists(SampleHelpers::AppBarTileId))
        {
            // First prepare the tile to be unpinned
            SecondaryTile secondaryTile(SampleHelpers::AppBarTileId);

            // Now make the delete request.
            bool isUnpinned = co_await secondaryTile.RequestDeleteForSelectionAsync(SampleHelpers::GetElementRect(sender), Windows::UI::Popups::Placement::Above);
            if (isUnpinned)
            {
                Message().Text(hstring(SampleHelpers::AppBarTileId) + L" was unpinned.");
            }
            else
            {
                Message().Text(hstring(SampleHelpers::AppBarTileId) + L" was not unpinned.");
            }

            UpdateAppBarButton();
        }
        else
        {
            // Prepare package images for the medium tile size in our tile to be pinned
            Uri square150x150Logo(L"ms-appx:///Assets/square150x150Tile-sdk.png");

            // During creation of secondary tile, an application may set additional arguments on the tile that will be passed in during activation,
            // so that the app knows which tile the user is launching. In this sample, we'll pass in the date and time the secondary tile was pinned.
            hstring tileActivationArguments = hstring(SampleHelpers::AppBarTileId) + L" WasPinnedAt=" + SampleHelpers::CurrentTimeAsString();

            // Create a Secondary tile with all the required arguments.
            // Note the last argument specifies what size the Secondary tile should show up as by default in the Pin to start fly out.
            // It can be set to TileSize.Square150x150, TileSize.Wide310x150, or TileSize.Default.  
            // If set to TileSize.Wide310x150, then the asset for the wide size must be supplied as well.  
            // TileSize.Default will default to the wide size if a wide size is provided, and to the medium size otherwise. 
            SecondaryTile secondaryTile(SampleHelpers::AppBarTileId,
                L"Secondary tile pinned via AppBar",
                tileActivationArguments,
                square150x150Logo,
                TileSize::Square150x150);

            // Whether or not the app name should be displayed on the tile can be controlled for each tile size.  The default is false.
            secondaryTile.VisualElements().ShowNameOnSquare150x150Logo(true);

            // Specify a foreground text value.
            // The tile background color is inherited from the parent unless a separate value is specified.
            secondaryTile.VisualElements().ForegroundText(ForegroundText::Dark);

            // OK, the tile is created and we can now attempt to pin the tile.
            // Note that the status message is updated when the async operation to pin the tile completes.
            bool isPinned = co_await secondaryTile.RequestCreateForSelectionAsync(SampleHelpers::GetElementRect(sender), Windows::UI::Popups::Placement::Above);
            if (isPinned)
            {
                Message().Text(hstring(SampleHelpers::AppBarTileId) + L" was successfully pinned.");
            }
            else
            {
                Message().Text(hstring(SampleHelpers::AppBarTileId) + L" was not pinned.");
            }

            UpdateAppBarButton();
        }

        BottomAppBar().IsSticky(false);
    }

    void Scenario7_PinFromAppbar::BottomAppBar_Opened(IInspectable const&, IInspectable const&)
    {
        UpdateAppBarButton();
    }

}
