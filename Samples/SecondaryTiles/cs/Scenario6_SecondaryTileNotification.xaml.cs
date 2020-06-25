//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using NotificationsExtensions;
using NotificationsExtensions.Badges;
using NotificationsExtensions.Tiles;
using System;
using Windows.UI.Notifications;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario6_SecondaryTileNotification : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario6_SecondaryTileNotification()
        {
            this.InitializeComponent();
        }

        private bool VerifyTileIsPinned()
        {
            if (SecondaryTile.Exists(MainPage.dynamicTileId))
            {
                return true;
            }
            rootPage.NotifyUser(MainPage.dynamicTileId + " is not pinned.", NotifyType.ErrorMessage);
            return false;
        }

        private async void PinLiveTile_Click(object sender, RoutedEventArgs e)
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

        private void SendTileNotification_Click(object sender, RoutedEventArgs e)
        {
            if (VerifyTileIsPinned())
            {
                var tileContent = new TileContent
                {
                    Visual = new TileVisual
                    {
                        TileWide = new TileBinding
                        {
                            Content = new TileBindingContentAdaptive
                            {
                                Children =
                                    {
                                        new AdaptiveText{ Text = "Sent to a secondary tile from NotificationsExtensions!", HintWrap = true },
                                    }
                            }
                        }
                    }
                };
                // Send the notification to the secondary tile by creating a secondary tile updater
                TileUpdateManager.CreateTileUpdaterForSecondaryTile(MainPage.dynamicTileId).Update(new TileNotification(tileContent.GetXml()));

                rootPage.NotifyUser("Tile notification sent to " + MainPage.dynamicTileId, NotifyType.StatusMessage);
            }
        }

        private void SendBadgeNotification_Click(object sender, RoutedEventArgs e)
        {
            if (VerifyTileIsPinned())
            {
                BadgeNumericNotificationContent badgeContent = new BadgeNumericNotificationContent(6);

                // Send the notification to the secondary tile
                BadgeUpdateManager.CreateBadgeUpdaterForSecondaryTile(MainPage.dynamicTileId).Update(new BadgeNotification(badgeContent.GetXml()));

                rootPage.NotifyUser("Badge notification sent to " + MainPage.dynamicTileId, NotifyType.StatusMessage);
            }
        }

        private void SendTileNotificationWithStringManipulation_Click(object sender, RoutedEventArgs e)
        {
            if (VerifyTileIsPinned())
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

                var tileDOM = new Windows.Data.Xml.Dom.XmlDocument();
                tileDOM.LoadXml(tileXmlString);
                TileNotification tile = new TileNotification(tileDOM);

                // Send the notification to the secondary tile by creating a secondary tile updater
                TileUpdateManager.CreateTileUpdaterForSecondaryTile(MainPage.dynamicTileId).Update(tile);

                rootPage.NotifyUser("Tile notification sent to " + MainPage.dynamicTileId, NotifyType.StatusMessage);
            }
        }

        private void SendBadgeNotificationWithStringManipulation_Click(object sender, RoutedEventArgs e)
        {
            if (VerifyTileIsPinned())
            {
                string badgeXmlString = "<badge value='9'/>";
                var badgeDOM = new Windows.Data.Xml.Dom.XmlDocument();
                badgeDOM.LoadXml(badgeXmlString);
                BadgeNotification badge = new BadgeNotification(badgeDOM);

                // Send the notification to the secondary tile
                BadgeUpdateManager.CreateBadgeUpdaterForSecondaryTile(MainPage.dynamicTileId).Update(badge);

                rootPage.NotifyUser("Badge notification sent to " + MainPage.dynamicTileId, NotifyType.StatusMessage);
            }
        }
    }
}
