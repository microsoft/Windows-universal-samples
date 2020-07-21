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
using SDKTemplate;
using Windows.UI;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SecondaryTiles
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class PinTileAlternateVisualElements : Page
    {

        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        AppBar appBar;

        public PinTileAlternateVisualElements()
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
                // Prepare package images for our tile to be pinned
                Uri square70x70Logo = new Uri("ms-appx:///Assets/square70x70Tile-sdk.png");
                Uri square150x150Logo = new Uri("ms-appx:///Assets/square150x150Tile-sdk.png");
                Uri wide310x150Logo = new Uri("ms-appx:///Assets/wide310x150Tile-sdk.png");
                Uri square310x310Logo = new Uri("ms-appx:///Assets/square310x310Tile-sdk.png");
                Uri square30x30Logo = new Uri("ms-appx:///Assets/square30x30Tile-sdk.png");

                // During creation of the secondary tile, an application may set additional arguments on the tile that will be passed in during activation.
                // These arguments should be meaningful to the application. In this sample, we'll pass in the date and time the secondary tile was pinned.
                string tileActivationArguments = MainPage.logoSecondaryTileId + " WasPinnedAt=" + DateTime.Now.ToLocalTime().ToString();

                // Create a Secondary tile with all the required arguments and set the preferred size to be the medium tile size.
                SecondaryTile secondaryTile = new SecondaryTile(MainPage.logoSecondaryTileId,
                                                                "Title text shown on the tile",
                                                                tileActivationArguments,
                                                                square150x150Logo,
                                                                TileSize.Square150x150);

                // Only support of the small and medium tile sizes is mandatory.
                // To have the larger tile sizes available the assets must be provided.
                secondaryTile.VisualElements.Wide310x150Logo = wide310x150Logo;
                secondaryTile.VisualElements.Square310x310Logo = square310x310Logo;

                // If the asset for the small tile size is not provided, it will be created by scaling down the medium tile size asset.
                // Thus, providing the asset for the small tile size is not mandatory, though is recommended to avoid scaling artifacts and can be overridden as shown below. 
                // Note that the asset for the small tile size must be explicitly provided if alternates for the small tile size are also explicitly provided.
                secondaryTile.VisualElements.Square70x70Logo = square70x70Logo;

                // Like the background color, the square30x30 logo is inherited from the parent application tile by default. 
                // Let's override it, just to see how that's done.
                secondaryTile.VisualElements.Square30x30Logo = square30x30Logo;
                
                // The display of the secondary tile name can be controlled for each tile size.
                secondaryTile.VisualElements.ShowNameOnSquare150x150Logo = false;
                secondaryTile.VisualElements.ShowNameOnWide310x150Logo = true;
                secondaryTile.VisualElements.ShowNameOnSquare310x310Logo = true;

                // Add the handler for the VisualElemets request.
                // This is needed to add alternate tile options for a user to choose from for the supported tile sizes.
                secondaryTile.VisualElementsRequested += VisualElementsRequestedHandler;

                // Specify a foreground text value.
                // The tile background color is inherited from the parent unless a separate value is specified.
                secondaryTile.VisualElements.ForegroundText = ForegroundText.Dark;

                // OK, the tile is created and we can now attempt to pin the tile.
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
        }

        // Alternate tile options for the supported tile sizes are set in the handler.
        void VisualElementsRequestedHandler(object sender, VisualElementsRequestedEventArgs args)
        {
            // Prepare the images for the alternate tile options 
            Uri alternate1Square70x70Logo = new Uri("ms-appx:///Assets/alternate1Square70x70Tile-sdk.png");
            Uri alternate1Square150x150Logo = new Uri("ms-appx:///Assets/alternate1Square150x150Tile-sdk.png");
            Uri alternate1Wide310x150Logo = new Uri("ms-appx:///Assets/alternate1Wide310x150Tile-sdk.png");
            Uri alternate1Square310x310Logo = new Uri("ms-appx:///Assets/alternate1Square310x310Tile-sdk.png");

            Uri alternate2Square70x70Logo = new Uri("ms-appx:///Assets/alternate2Square70x70Tile-sdk.png");
            Uri alternate2Square150x150Logo = new Uri("ms-appx:///Assets/alternate2Square150x150Tile-sdk.png");
            Uri alternate2Wide310x150Logo = new Uri("ms-appx:///Assets/alternate2Wide310x150Tile-sdk.png");
            Uri alternate2Square310x310Logo = new Uri("ms-appx:///Assets/alternate2Square310x310Tile-sdk.png");

            Uri alternate3Square70x70Logo = new Uri("ms-appx:///Assets/alternate3Square70x70Tile-sdk.png");
            Uri alternate3Square150x150Logo = new Uri("ms-appx:///Assets/alternate3Square150x150Tile-sdk.png");
            Uri alternate3Wide310x150Logo = new Uri("ms-appx:///Assets/alternate3Wide310x150Tile-sdk.png");
            Uri alternate3Square310x310Logo = new Uri("ms-appx:///Assets/alternate3Square310x310Tile-sdk.png");

            // Set the assets for the alternate tile options.
            // Note that up to 3 sets of alternate options can be provided.
            // Each tile size in a given set will map to each other (for example, the second alternate medium tile option will map to the second alternate wide tile option).
            // Note that if alternates are provided for the small tile size (square70x70 logo), the default option for the small tile size must be overridden as shown earlier. 
            // If neither the default option nor the alternates for the small tile size are provided, then all the medium tile size options will be scaled down to their corresponding small tile size options. 
            args.Request.AlternateVisualElements[0].Square70x70Logo = alternate1Square70x70Logo;
            args.Request.AlternateVisualElements[0].Square150x150Logo = alternate1Square150x150Logo;
            args.Request.AlternateVisualElements[0].Wide310x150Logo = alternate1Wide310x150Logo;
            args.Request.AlternateVisualElements[0].Square310x310Logo = alternate1Square310x310Logo;

            // Set the BackgoundColor, ForegroundText, and ShowName propreties for the alternate set.
            args.Request.AlternateVisualElements[0].BackgroundColor = Colors.Green;
            args.Request.AlternateVisualElements[0].ForegroundText = ForegroundText.Dark;
            args.Request.AlternateVisualElements[0].ShowNameOnSquare310x310Logo = true;

            args.Request.AlternateVisualElements[1].Square70x70Logo = alternate2Square70x70Logo;
            args.Request.AlternateVisualElements[1].Square150x150Logo = alternate2Square150x150Logo;
            args.Request.AlternateVisualElements[1].Wide310x150Logo = alternate2Wide310x150Logo;
            args.Request.AlternateVisualElements[1].Square310x310Logo = alternate2Square310x310Logo;

            args.Request.AlternateVisualElements[1].BackgroundColor = Color.FromArgb(0xFF, 0x51, 0x33, 0xAB); ;
            args.Request.AlternateVisualElements[1].ForegroundText = ForegroundText.Dark;
            args.Request.AlternateVisualElements[1].ShowNameOnWide310x150Logo = true;

            args.Request.AlternateVisualElements[2].Square70x70Logo = alternate3Square70x70Logo;
            args.Request.AlternateVisualElements[2].Square150x150Logo = alternate3Square150x150Logo;
            args.Request.AlternateVisualElements[2].Wide310x150Logo = alternate3Wide310x150Logo;
            args.Request.AlternateVisualElements[2].Square310x310Logo = alternate3Square310x310Logo;

            args.Request.AlternateVisualElements[2].BackgroundColor = Colors.Red;
            args.Request.AlternateVisualElements[2].ForegroundText = ForegroundText.Light;
            args.Request.AlternateVisualElements[2].ShowNameOnSquare150x150Logo = true;
        }
    }
}