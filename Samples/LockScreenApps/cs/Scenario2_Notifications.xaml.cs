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

using NotificationsExtensions;
using NotificationsExtensions.Badges;
using NotificationsExtensions.Tiles;
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario2_Notifications : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario2_Notifications()
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
            BadgeUpdateManager.CreateBadgeUpdaterForApplication().Update(new BadgeNotification(badgeContent.GetXml()));
            rootPage.NotifyUser("Badge notification sent", NotifyType.StatusMessage);
        }

        private void SendBadgeWithStringManipulation_Click(object sender, RoutedEventArgs e)
        {
            string badgeXmlString = "<badge value='6'/>";
            XmlDocument badgeDOM = new XmlDocument();
            badgeDOM.LoadXml(badgeXmlString);
            BadgeUpdateManager.CreateBadgeUpdaterForApplication().Update(new BadgeNotification(badgeDOM));
            rootPage.NotifyUser("Badge notification sent", NotifyType.StatusMessage);
        }

        private void SendTile_Click(object sender, RoutedEventArgs e)
        {
            var tileDOM = TileUpdateManager.GetTemplateContent(TileTemplateType.TileWide310x150SmallImageAndText03);
            tileDOM.SelectSingleNode("//image[@id=1]/@src").NodeValue = "'ms-appx:///Assets/tile-sdk.png";
            tileDOM.SelectSingleNode("//text[@id=1]").InnerText = "This tile notification has an image, but it won't be displayed on the lock screen";

            TileUpdateManager.CreateTileUpdaterForApplication().Update(new TileNotification(tileDOM));

            rootPage.NotifyUser("Tile notification sent", NotifyType.StatusMessage);
        }

        private void SendTileWithStringManipulation_Click(object sender, RoutedEventArgs e)
        {
            string tileXmlString = "<tile>"
                                 + "<visual version='2'>"
                                 + "<binding template='TileWide310x150SmallImageAndText03' fallback='TileWideSmallImageAndText03'>"
                                 + "<image id='1' src='ms-appx:///Assets/tile-sdk.png'/>"
                                 + "<text id='1'>This tile notification has an image, but it won't be displayed on the lock screen</text>"
                                 + "</binding>"
                                 + "</visual>"
                                 + "</tile>";

            XmlDocument tileDOM = new XmlDocument();
            tileDOM.LoadXml(tileXmlString);
            TileUpdateManager.CreateTileUpdaterForApplication().Update(new TileNotification(tileDOM));
            rootPage.NotifyUser("Tile notification sent", NotifyType.StatusMessage);
        }

        private void ClearTile_Click(object sender, RoutedEventArgs e)
        {
            TileUpdateManager.CreateTileUpdaterForApplication().Clear();
            rootPage.NotifyUser("Tile notification cleared", NotifyType.StatusMessage);
        }
    }
}
