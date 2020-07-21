//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using SDKTemplate;
using System;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SecondaryTiles
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class PinTile : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        AppBar appBar;

        public PinTile()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Preserve the app bar
            appBar = rootPage.BottomAppBar;
            // this ensures the app bar is not shown in this scenario
            rootPage.BottomAppBar = null;
        }

        /// <summary>
        /// Invoked when this page is about to be navigated out in a Frame
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            // Restore the app bar
            rootPage.BottomAppBar = appBar;
        }
        /// <summary>
        /// This is the click handler for the 'PinButton' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void PinButton_Click(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;
            if (button != null)
            {
                // Prepare package images for all four tile sizes in our tile to be pinned as well as for the square30x30 logo used in the Apps view.  
                Uri square150x150Logo = new Uri("ms-appx:///Assets/square150x150Tile-sdk.png");
                Uri wide310x150Logo = new Uri("ms-appx:///Assets/wide310x150Tile-sdk.png");
                Uri square310x310Logo = new Uri("ms-appx:///Assets/square310x310Tile-sdk.png");
                Uri square30x30Logo = new Uri("ms-appx:///Assets/square30x30Tile-sdk.png");

                // During creation of secondary tile, an application may set additional arguments on the tile that will be passed in during activation.
                // These arguments should be meaningful to the application. In this sample, we'll pass in the date and time the secondary tile was pinned.
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

                if(!(Windows.Foundation.Metadata.ApiInformation.IsTypePresent(("Windows.Phone.UI.Input.HardwareButtons"))))
                {
                    secondaryTile.VisualElements.Wide310x150Logo = wide310x150Logo;
                    secondaryTile.VisualElements.Square310x310Logo = square310x310Logo;
                }

                // Like the background color, the square30x30 logo is inherited from the parent application tile by default. 
                // Let's override it, just to see how that's done.
                secondaryTile.VisualElements.Square30x30Logo = square30x30Logo;

                // The display of the secondary tile name can be controlled for each tile size.
                // The default is false.
                secondaryTile.VisualElements.ShowNameOnSquare150x150Logo = true;

                if (!(Windows.Foundation.Metadata.ApiInformation.IsTypePresent(("Windows.Phone.UI.Input.HardwareButtons"))))
                {
                    secondaryTile.VisualElements.ShowNameOnWide310x150Logo = true;
                    secondaryTile.VisualElements.ShowNameOnSquare310x310Logo = true;
                }

                // Specify a foreground text value.
                // The tile background color is inherited from the parent unless a separate value is specified.
                secondaryTile.VisualElements.ForegroundText = ForegroundText.Dark;

                // Set this to false if roaming doesn't make sense for the secondary tile.
                // The default is true;
                secondaryTile.RoamingEnabled = false;

                // OK, the tile is created and we can now attempt to pin the tile.
                if (!(Windows.Foundation.Metadata.ApiInformation.IsTypePresent(("Windows.Phone.UI.Input.HardwareButtons"))))
                {
                    // Note that the status message is updated when the async operation to pin the tile completes.
                    bool isPinned = await secondaryTile.RequestCreateForSelectionAsync(MainPage.GetElementRect((FrameworkElement)sender), Windows.UI.Popups.Placement.Below);

                    if (isPinned)
                    {
                        rootPage.NotifyUser("Secondary tile successfully pinned.", NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser("Secondary tile not pinned.", NotifyType.ErrorMessage);
                    }
                }
                if (Windows.Foundation.Metadata.ApiInformation.IsTypePresent(("Windows.Phone.UI.Input.HardwareButtons")))
                {
                    // OK, the tile is created and we can now attempt to pin the tile.
                    // Since pinning a secondary tile on Windows Phone will exit the app and take you to the start screen, any code after 
                    // RequestCreateForSelectionAsync or RequestCreateAsync is not guaranteed to run.  For an example of how to use the OnSuspending event to do
                    // work after RequestCreateForSelectionAsync or RequestCreateAsync returns, see Scenario9_PinTileAndUpdateOnSuspend in the SecondaryTiles.WindowsPhone project.
                    await secondaryTile.RequestCreateAsync();
                }
            }
        }
    }
}
