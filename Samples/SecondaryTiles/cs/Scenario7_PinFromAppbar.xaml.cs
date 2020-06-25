//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using Windows.UI.Core;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario7_PinFromAppbar : Page
    {
        // Don't use NotifyUser on this page because it conflicts with the AppBar.

        public Scenario7_PinFromAppbar()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // The appbar is open by default.
            Message.Text = "Tap or click the Pin button to pin a tile.";
            UpdateAppBarButton();

            Window.Current.Activated += Window_Activated;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            Window.Current.Activated -= Window_Activated;
        }

        void Window_Activated(object sender, WindowActivatedEventArgs e)
        {
            // When the window regains activation, refresh the pin/unpin button.
            UpdateAppBarButton();
        }
 
        // This updates the Pin/Unpin button in the app bar based on whether the
        // secondary tile is already pinned.
        private void UpdateAppBarButton()
        {
            if (SecondaryTile.Exists(MainPage.appbarTileId))
            {
                this.PinUnPinCommandButton.Label = "Unpin";
                this.PinUnPinCommandButton.Icon = new SymbolIcon(Symbol.UnPin);
            }
            else
            {
                this.PinUnPinCommandButton.Label = "Pin";
                this.PinUnPinCommandButton.Icon = new SymbolIcon(Symbol.Pin);
            }

            this.PinUnPinCommandButton.UpdateLayout();
        }

        async void PinToAppBar_Click(object sender, RoutedEventArgs e)
        {
            this.SecondaryTileCommandBar.IsSticky = true;

            // Let us first verify if we need to pin or unpin
            if (SecondaryTile.Exists(MainPage.appbarTileId))
            {
                // First prepare the tile to be unpinned
                SecondaryTile secondaryTile = new SecondaryTile(MainPage.appbarTileId);

                // Now make the delete request.
                bool isUnpinned = await secondaryTile.RequestDeleteForSelectionAsync(MainPage.GetElementRect((FrameworkElement)sender), Windows.UI.Popups.Placement.Above);
                if (isUnpinned)
                {
                    Message.Text = MainPage.appbarTileId + " was unpinned.";
                }
                else
                {
                    Message.Text = MainPage.appbarTileId + " was not unpinned.";
                }

                UpdateAppBarButton();
            }
            else
            {
                // Prepare package images for the medium tile size in our tile to be pinned
                Uri square150x150Logo = new Uri("ms-appx:///Assets/square150x150Tile-sdk.png");

                // During creation of secondary tile, an application may set additional arguments on the tile that will be passed in during activation,
                // so that the app knows which tile the user is launching. In this sample, we'll pass in the date and time the secondary tile was pinned.
                string tileActivationArguments = MainPage.appbarTileId + " WasPinnedAt=" + DateTime.Now.ToLocalTime().ToString();

                // Create a Secondary tile with all the required arguments.
                // Note the last argument specifies what size the Secondary tile should show up as by default in the Pin to start fly out.
                // It can be set to TileSize.Square150x150, TileSize.Wide310x150, or TileSize.Default.  
                // If set to TileSize.Wide310x150, then the asset for the wide size must be supplied as well.  
                // TileSize.Default will default to the wide size if a wide size is provided, and to the medium size otherwise. 
                SecondaryTile secondaryTile = new SecondaryTile(MainPage.appbarTileId,
                                                                "Secondary tile pinned via AppBar",
                                                                tileActivationArguments,
                                                                square150x150Logo,
                                                                TileSize.Square150x150);

                // Whether or not the app name should be displayed on the tile can be controlled for each tile size.  The default is false.
                secondaryTile.VisualElements.ShowNameOnSquare150x150Logo = true;

                // Specify a foreground text value.
                // The tile background color is inherited from the parent unless a separate value is specified.
                secondaryTile.VisualElements.ForegroundText = ForegroundText.Dark;

                // OK, the tile is created and we can now attempt to pin the tile.
                // Note that the status message is updated when the async operation to pin the tile completes.
                bool isPinned = await secondaryTile.RequestCreateForSelectionAsync(MainPage.GetElementRect(sender), Windows.UI.Popups.Placement.Above);
                if (isPinned)
                {
                    Message.Text = MainPage.appbarTileId + " was successfully pinned.";
                }
                else
                {
                    Message.Text = MainPage.appbarTileId + " was not pinned.";
                }

                UpdateAppBarButton();
            }

            this.BottomAppBar.IsSticky = false;
        }

        void BottomAppBar_Opened(object sender, object e)
        {
            UpdateAppBarButton();
        }
    }
}
