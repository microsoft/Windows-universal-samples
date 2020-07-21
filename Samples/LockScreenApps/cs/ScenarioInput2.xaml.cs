// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

using NotificationsExtensions.BadgeContent;
using NotificationsExtensions.TileContent;
using System;
using Windows.UI.Notifications;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplateCS;

namespace LockScreenAppsCS
{
    public sealed partial class ScenarioInput2 : Page
    {
        // A pointer back to the main page which is used to gain access to the input and output frames and their content.
        MainPage rootPage = null;

        public ScenarioInput2()
        {
            InitializeComponent();
        }

        private void ClearBadge_Click(object sender, RoutedEventArgs e)
        {
            BadgeUpdateManager.CreateBadgeUpdaterForApplication().Clear();
            rootPage.NotifyUser("Badge notification cleared", NotifyType.StatusMessage);
        }

        private void SendBadge_Click(object sender, RoutedEventArgs e)
        {
            BadgeNumericNotificationContent badgeContent = new BadgeNumericNotificationContent(6);
            BadgeUpdateManager.CreateBadgeUpdaterForApplication().Update(badgeContent.CreateNotification());
            rootPage.NotifyUser("Badge notification sent", NotifyType.StatusMessage);
        }

        private void SendBadgeWithStringManipulation_Click(object sender, RoutedEventArgs e)
        {
            string badgeXmlString = "<badge value='6'/>";
            Windows.Data.Xml.Dom.XmlDocument badgeDOM = new Windows.Data.Xml.Dom.XmlDocument();
            badgeDOM.LoadXml(badgeXmlString);
            BadgeNotification badge = new BadgeNotification(badgeDOM);
            BadgeUpdateManager.CreateBadgeUpdaterForApplication().Update(badge);
            rootPage.NotifyUser("Badge notification sent", NotifyType.StatusMessage);
        }

        private void SendTile_Click(object sender, RoutedEventArgs e)
        {
            ITileWide310x150SmallImageAndText03 tileContent = TileContentFactory.CreateTileWide310x150SmallImageAndText03();
            tileContent.TextBodyWrap.Text = "This tile notification has an image, but it won't be displayed on the lock screen";
            tileContent.Image.Src = "ms-appx:///images/tile-sdk.png";
            tileContent.RequireSquare150x150Content = false;
            TileUpdateManager.CreateTileUpdaterForApplication().Update(tileContent.CreateNotification());

            rootPage.NotifyUser("Tile notification sent", NotifyType.StatusMessage);
        }

        private void SendTileWithStringManipulation_Click(object sender, RoutedEventArgs e)
        {
            string tileXmlString = "<tile>"
                                 + "<visual version='2'>"
                                 + "<binding template='TileWide310x150SmallImageAndText03' fallback='TileWideSmallImageAndText03'>"
                                 + "<image id='1' src='ms-appx:///images/tile-sdk.png'/>"
                                 + "<text id='1'>This tile notification has an image, but it won't be displayed on the lock screen</text>"
                                 + "</binding>"
                                 + "</visual>"
                                 + "</tile>";

            Windows.Data.Xml.Dom.XmlDocument tileDOM = new Windows.Data.Xml.Dom.XmlDocument();
            tileDOM.LoadXml(tileXmlString);
            TileNotification tile = new TileNotification(tileDOM);
            TileUpdateManager.CreateTileUpdaterForApplication().Update(tile);
            rootPage.NotifyUser("Tile notification sent", NotifyType.StatusMessage);
        }

        private void ClearTile_Click(object sender, RoutedEventArgs e)
        {
            TileUpdateManager.CreateTileUpdaterForApplication().Clear();
            rootPage.NotifyUser("Tile notification cleared", NotifyType.StatusMessage);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Get a pointer to our main page
            rootPage = e.Parameter as MainPage;
        }
    }
}
