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
    public sealed partial class PinFromAppbar : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public PinFromAppbar()
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
            Init();
        }


        // This toggles the Pin and unpin button in the app bar
        private void ToggleAppBarButton(bool showPinButton)
        {
            if (showPinButton)
            {
                this.PinUnPinCommandButton.Label = "Pin";
                this.PinUnPinCommandButton.Icon = new SymbolIcon(Symbol.Pin);
            }
            else
            {
                this.PinUnPinCommandButton.Label = "Unpin";
                this.PinUnPinCommandButton.Icon = new SymbolIcon(Symbol.UnPin);
            }

            this.PinUnPinCommandButton.UpdateLayout();
        }

        void Init()
        {
            // The appbar is open by default.
            rootPage.NotifyUser("Tap or click the Pin button to pin a tile.", NotifyType.StatusMessage);
            ToggleAppBarButton(!SecondaryTile.Exists(MainPage.appbarTileId));
            this.PinUnPinCommandButton.Click += this.pinToAppBar_Click;
        }

        async void pinToAppBar_Click(object sender, RoutedEventArgs e)
        {
            if (!(Windows.Foundation.Metadata.ApiInformation.IsTypePresent(("Windows.Phone.UI.Input.HardwareButtons"))))
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
                    rootPage.NotifyUser(MainPage.appbarTileId + " unpinned.", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser(MainPage.appbarTileId + " not unpinned.", NotifyType.ErrorMessage);
                }

                ToggleAppBarButton(isUnpinned);
            }
            else
            {
                // Prepare package images for the medium tile size in our tile to be pinned
                Uri square150x150Logo = new Uri("ms-appx:///Assets/square150x150Tile-sdk.png");

                // During creation of secondary tile, an application may set additional arguments on the tile that will be passed in during activation.
                // These arguments should be meaningful to the application. In this sample, we'll pass in the date and time the secondary tile was pinned.
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
                if (!(Windows.Foundation.Metadata.ApiInformation.IsTypePresent(("Windows.Phone.UI.Input.HardwareButtons"))))
                {
                    // Note that the status message is updated when the async operation to pin the tile completes.
                    bool isPinned = await secondaryTile.RequestCreateForSelectionAsync(MainPage.GetElementRect((FrameworkElement)sender), Windows.UI.Popups.Placement.Above);
                    if (isPinned)
                    {
                        rootPage.NotifyUser(MainPage.appbarTileId + " successfully pinned.", NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser(MainPage.appbarTileId + " not pinned.", NotifyType.ErrorMessage);
                    }

                    ToggleAppBarButton(!isPinned);
                }

                if ((Windows.Foundation.Metadata.ApiInformation.IsTypePresent(("Windows.Phone.UI.Input.HardwareButtons"))))
                {
                    // Since pinning a secondary tile on Windows Phone will exit the app and take you to the start screen, any code after 
                    // RequestCreateForSelectionAsync or RequestCreateAsync is not guaranteed to run.  For an example of how to use the OnSuspending event to do
                    // work after RequestCreateForSelectionAsync or RequestCreateAsync returns, see Scenario9_PinTileAndUpdateOnSuspend in the SecondaryTiles.WindowsPhone project.
                    await secondaryTile.RequestCreateAsync();
                }
            }

            if (!(Windows.Foundation.Metadata.ApiInformation.IsTypePresent(("Windows.Phone.UI.Input.HardwareButtons"))))
                this.BottomAppBar.IsSticky = false;
        }

        void BottomAppBar_Opened(object sender, object e)
        {
            ToggleAppBarButton(!SecondaryTile.Exists(MainPage.appbarTileId));
        }
    }
}
