//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using NotificationsExtensions.BadgeContent;
using NotificationsExtensions.TileContent;
using SDKTemplate;
using System;
using Windows.UI.Notifications;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SecondaryTiles
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class SecondaryTileNotification : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        AppBar appBar;

        public SecondaryTileNotification()
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
            ToggleButtons(SecondaryTile.Exists(MainPage.dynamicTileId));
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
        /// Enables or disables the notification buttons
        /// </summary>
        /// <param name="isEnabled"> enables if true</param>
        private void ToggleButtons(bool isEnabled)
        {
            SendBadgeNotification.IsEnabled = isEnabled;
            SendTileNotification.IsEnabled = isEnabled;
            SendBadgeNotificationString.IsEnabled = isEnabled;
            SendTileNotificationString.IsEnabled = isEnabled;
        }

        /// <summary>
        /// This is the click handler for the 'Pin Tile' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void PinLiveTile_Click(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;
            if (button != null)
            {
                // Prepare the images for our tile to be pinned.
                Uri square150x150Logo = new Uri("ms-appx:///Assets/square150x150Tile-sdk.png");
                Uri wide310x150Logo = new Uri("ms-appx:///Assets/wide310x150Tile-sdk.png");

                // During creation of the secondary tile, an application may set additional arguments on the tile that will be passed in during activation.
                // These arguments should be meaningful to the application. In this sample, we'll pass in the date and time the secondary tile was pinned.
                string tileActivationArguments = MainPage.dynamicTileId + " WasPinnedAt=" + DateTime.Now.ToLocalTime().ToString();

                // Create a Secondary tile with all the required properties and sets perfered size to Wide310x150.
                SecondaryTile secondaryTile = new SecondaryTile(MainPage.dynamicTileId,
                                                                "A Live Secondary Tile",
                                                                tileActivationArguments,
                                                                square150x150Logo,
                                                                TileSize.Wide310x150);

                // Adding the wide tile logo.
                secondaryTile.VisualElements.Wide310x150Logo = wide310x150Logo;

                // The display of the app name can be controlled for each tile size.
                secondaryTile.VisualElements.ShowNameOnSquare150x150Logo = true;
                secondaryTile.VisualElements.ShowNameOnWide310x150Logo = true;

                // Specify a foreground text value.
                // The tile background color is inherited from the parent unless a separate value is specified.
                secondaryTile.VisualElements.ForegroundText = ForegroundText.Dark;

                // OK, the tile is created and we can now attempt to pin the tile.
                if (!(Windows.Foundation.Metadata.ApiInformation.IsTypePresent(("Windows.Phone.UI.Input.HardwareButtons"))))
                {
                    // Note that the status message is updated when the async operation to pin the tile completes.
                    bool isPinned = await secondaryTile.RequestCreateForSelectionAsync(MainPage.GetElementRect((FrameworkElement)sender), Windows.UI.Popups.Placement.Below);

                    if (isPinned)
                    {
                        rootPage.NotifyUser("Secondary tile successfully pinned.", NotifyType.StatusMessage);
                        ToggleButtons(true);
                    }
                    else
                    {
                        rootPage.NotifyUser("Secondary tile not pinned.", NotifyType.ErrorMessage);
                    }
                }
                if (Windows.Foundation.Metadata.ApiInformation.IsTypePresent(("Windows.Phone.UI.Input.HardwareButtons")))
                {
                    // Since pinning a secondary tile on Windows Phone will exit the app and take you to the start screen, any code after 
                    // RequestCreateForSelectionAsync or RequestCreateAsync is not guaranteed to run.  For an example of how to use the OnSuspending event to do
                    // work after RequestCreateForSelectionAsync or RequestCreateAsync returns, see Scenario9_PinTileAndUpdateOnSuspend in the SecondaryTiles.WindowsPhone project.
                    await secondaryTile.RequestCreateAsync();
                }
            }
        }

        /// <summary>
        /// This is the click handler for the 'Sending tile notification' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void SendTileNotification_Click(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;
            if (button != null)
            {
                if (SecondaryTile.Exists(MainPage.dynamicTileId))
                {
                    // Note: This sample contains an additional reference, NotificationsExtensions, which you can use in your apps
                    ITileWide310x150Text04 tileContent = TileContentFactory.CreateTileWide310x150Text04();
                    tileContent.TextBodyWrap.Text = "Sent to a secondary tile from NotificationsExtensions!";

                    ITileSquare150x150Text04 squareContent = TileContentFactory.CreateTileSquare150x150Text04();
                    squareContent.TextBodyWrap.Text = "Sent to a secondary tile from NotificationExtensions!";
                    tileContent.Square150x150Content = squareContent;

                    // Send the notification to the secondary tile by creating a secondary tile updater
                    TileUpdateManager.CreateTileUpdaterForSecondaryTile(MainPage.dynamicTileId).Update(tileContent.CreateNotification());

                    rootPage.NotifyUser("Tile notification sent to " + MainPage.dynamicTileId, NotifyType.StatusMessage);
                }
                else
                {
                    ToggleButtons(false);
                    rootPage.NotifyUser(MainPage.dynamicTileId + " not pinned.", NotifyType.ErrorMessage);
                }
            }
        }

        /// <summary>
        /// This is the click handler for the 'Other' button.  You would replace this with your own handler
        /// if you have a button or buttons on this page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void SendBadgeNotification_Click(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;
            if (button != null)
            {
                if (SecondaryTile.Exists(MainPage.dynamicTileId))
                {
                    BadgeNumericNotificationContent badgeContent = new BadgeNumericNotificationContent(6);

                    // Send the notification to the secondary tile
                    BadgeUpdateManager.CreateBadgeUpdaterForSecondaryTile(MainPage.dynamicTileId).Update(badgeContent.CreateNotification());

                    rootPage.NotifyUser("Badge notification sent to " + MainPage.dynamicTileId, NotifyType.StatusMessage);
                }
                else
                {
                    ToggleButtons(false);
                    rootPage.NotifyUser(MainPage.dynamicTileId + " not pinned.", NotifyType.ErrorMessage);
                }
            }
        }

        private void SendTileNotificationWithStringManipulation_Click(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;
            if (button != null)
            {
                string tileXmlString = "<tile>"
                                     + "<visual version='2'>"
                                     + "<binding template='TileWide310x150Text04' fallback='TileWideText04'>"
                                     + "<text id='1'>Send to a secondary tile from strings</text>"
                                     + "</binding>"
                                     + "<binding template='TileSquare150x150Text04' fallback='TileSquareText04'>"
                                     + "<text id='1'>Send to a secondary tile from strings</text>"
                                     + "</binding>"
                                     + "</visual>"
                                     + "</tile>";

                Windows.Data.Xml.Dom.XmlDocument tileDOM = new Windows.Data.Xml.Dom.XmlDocument();
                tileDOM.LoadXml(tileXmlString);
                TileNotification tile = new TileNotification(tileDOM);

                // Send the notification to the secondary tile by creating a secondary tile updater
                TileUpdateManager.CreateTileUpdaterForSecondaryTile(MainPage.dynamicTileId).Update(tile);

                rootPage.NotifyUser("Tile notification sent to " + MainPage.dynamicTileId, NotifyType.StatusMessage);
            }
        }

        private void SendBadgeNotificationWithStringManipulation_Click(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;
            if (button != null)
            {
                string badgeXmlString = "<badge value='9'/>";
                Windows.Data.Xml.Dom.XmlDocument badgeDOM = new Windows.Data.Xml.Dom.XmlDocument();
                badgeDOM.LoadXml(badgeXmlString);
                BadgeNotification badge = new BadgeNotification(badgeDOM);

                // Send the notification to the secondary tile
                BadgeUpdateManager.CreateBadgeUpdaterForSecondaryTile(MainPage.dynamicTileId).Update(badge);

                rootPage.NotifyUser("Badge notification sent to " + MainPage.dynamicTileId, NotifyType.StatusMessage);
            }
        }
    }
}
