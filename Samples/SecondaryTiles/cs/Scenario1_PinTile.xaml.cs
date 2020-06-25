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
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario1_PinTile : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario1_PinTile()
        {
            this.InitializeComponent();
        }

        private async void PinButton_Click(object sender, RoutedEventArgs e)
        {
            // Prepare package images for the tile sizes in our tile to be pinned as well as for the square30x30 logo used in the Apps view.  
            Uri square150x150Logo = new Uri("ms-appx:///Assets/square150x150Tile-sdk.png");
            Uri wide310x150Logo = new Uri("ms-appx:///Assets/wide310x150Tile-sdk.png");
            Uri square310x310Logo = new Uri("ms-appx:///Assets/square310x310Tile-sdk.png");

            // During creation of secondary tile, an application may set additional arguments on the tile that will be passed in during activation,
            // so that the app knows which tile the user is launching. In this sample, we'll pass in the date and time the secondary tile was pinned.
            string tileActivationArguments = MainPage.logoSecondaryTileId + " WasPinnedAt=" + DateTime.Now.ToLocalTime().ToString();

            // Create a Secondary tile with all the required arguments.
            // Note the last argument specifies what size the Secondary tile should show up as by default in the Pin to start fly out.
            // It can be set to TileSize.Square150x150, TileSize.Wide310x150, or TileSize.Default.  
            // If set to TileSize.Wide310x150, then the asset for the wide size must be supplied as well.
            // TileSize.Default will default to the wide size if a wide size is provided, and to the medium size otherwise. 
            SecondaryTile secondaryTile = new SecondaryTile(MainPage.logoSecondaryTileId,
                                                            "Title text shown on the tile",
                                                            tileActivationArguments,
                                                            square150x150Logo,
                                                            TileSize.Square150x150);

            secondaryTile.VisualElements.Wide310x150Logo = wide310x150Logo;
            secondaryTile.VisualElements.Square310x310Logo = square310x310Logo;

            // The display of the secondary tile name can be controlled for each tile size.
            // The default is false.
            secondaryTile.VisualElements.ShowNameOnSquare150x150Logo = true;
            secondaryTile.VisualElements.ShowNameOnWide310x150Logo = true;
            secondaryTile.VisualElements.ShowNameOnSquare310x310Logo = true;

            // Specify a foreground text value.
            // The tile background color is inherited from the parent unless a separate value is specified.
            secondaryTile.VisualElements.ForegroundText = ForegroundText.Dark;

            // Set this to false if roaming doesn't make sense for the secondary tile.
            // The default is true.
            secondaryTile.RoamingEnabled = false;

            // The tile is created and we can now attempt to pin the tile.
            // Note that the status message is updated when the async operation to pin the tile completes.
            bool isPinned = await secondaryTile.RequestCreateForSelectionAsync(MainPage.GetElementRect(sender), Windows.UI.Popups.Placement.Below);

            if (isPinned)
            {
                rootPage.NotifyUser("Secondary tile successfully pinned.", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("Secondary tile not pinned.", NotifyType.ErrorMessage);
            }
        }
    }
}
