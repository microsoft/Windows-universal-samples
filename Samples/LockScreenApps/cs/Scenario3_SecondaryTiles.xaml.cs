// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

using NotificationsExtensions;
using NotificationsExtensions.Badges;
using NotificationsExtensions.Tiles;
using System;
using Windows.Foundation;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3_SecondaryTiles : Page
    {
        MainPage rootPage = MainPage.Current;
        private const String BADGE_TILE_ID = "ST_BADGE";
        private const String TEXT_TILE_ID = "ST_BADGE_AND_TEXT";

        public Scenario3_SecondaryTiles()
        {
            InitializeComponent();
        }

        private async void CreateBadgeTile_Click(object sender, RoutedEventArgs e)
        {
            if (!SecondaryTile.Exists(BADGE_TILE_ID)) {
                SecondaryTile secondTile = new SecondaryTile(
                    BADGE_TILE_ID,
                    "LockScreen CS - Badge only",
                    "BADGE_ARGS",
                    new Uri("ms-appx:///Assets/squareTile-sdk.png"),
                    TileSize.Square150x150
                );
                secondTile.LockScreenBadgeLogo = new Uri("ms-appx:///Assets/badgelogo-sdk.png");

                bool isPinned = await secondTile.RequestCreateForSelectionAsync(GetElementRect((FrameworkElement)sender), Placement.Above);
                if (isPinned)
                {
                    BadgeNumericNotificationContent badgeContent = new BadgeNumericNotificationContent(2);
                    BadgeUpdateManager.CreateBadgeUpdaterForSecondaryTile(BADGE_TILE_ID).Update(new BadgeNotification(badgeContent.GetXml()));
                    rootPage.NotifyUser("Secondary tile created and badge updated. Go to PC settings to add it to the lock screen.", NotifyType.StatusMessage);
                }
                else 
                {
                    rootPage.NotifyUser("Tile not created.", NotifyType.ErrorMessage);
                }
                
            } else {
                rootPage.NotifyUser("Badge secondary tile already exists.", NotifyType.ErrorMessage);
            }
        }

        private async void CreateBadgeAndTextTile_Click(object sender, RoutedEventArgs e)
        {
            if (!SecondaryTile.Exists(TEXT_TILE_ID))
            {
                SecondaryTile secondTile = new SecondaryTile(
                    TEXT_TILE_ID,
                    "LockScreen CS - Badge and tile text",
                    "TEXT_ARGS",
                    new Uri("ms-appx:///Assets/squareTile-sdk.png"),
                    TileSize.Wide310x150
                );
                secondTile.VisualElements.Wide310x150Logo = new Uri("ms-appx:///Assets/tile-sdk.png");
                secondTile.LockScreenBadgeLogo = new Uri("ms-appx:///Assets/badgelogo-sdk.png");
                secondTile.LockScreenDisplayBadgeAndTileText = true;

                bool isPinned = await secondTile.RequestCreateForSelectionAsync(GetElementRect((FrameworkElement)sender), Placement.Above);
                if (isPinned)
                {
                    var tileDOM = TileUpdateManager.GetTemplateContent(TileTemplateType.TileWide310x150Text03);
                    tileDOM.SelectSingleNode("//text[@id=1]").InnerText = "Text for the lock screen";

                    TileUpdateManager.CreateTileUpdaterForSecondaryTile(TEXT_TILE_ID).Update(new TileNotification(tileDOM));
                    rootPage.NotifyUser("Secondary tile created and updated. Go to PC settings to add it to the lock screen.", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Tile not created.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Badge and text secondary tile already exists.", NotifyType.ErrorMessage);
            }
        }

        private Rect GetElementRect(FrameworkElement element)
        {
            GeneralTransform buttonTransform = element.TransformToVisual(null);
            return buttonTransform.TransformBounds(new Rect(0, 0, element.ActualWidth, element.ActualHeight));
        }
    }
}
