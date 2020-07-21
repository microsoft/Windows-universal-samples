// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

using NotificationsExtensions.BadgeContent;
using NotificationsExtensions.TileContent;
using System;
using Windows.Foundation;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using SDKTemplateCS;

namespace LockScreenAppsCS
{
    public sealed partial class ScenarioInput3 : Page
    {
        // A pointer back to the main page which is used to gain access to the input and output frames and their content.
        MainPage rootPage = null;
        private const String BADGE_TILE_ID = "ST_BADGE";
        private const String TEXT_TILE_ID = "ST_BADGE_AND_TEXT";

        public ScenarioInput3()
        {
            InitializeComponent();
            CreateBadgeTile.Click += new RoutedEventHandler(CreateBadgeTile_Click);
            CreateBadgeAndTextTile.Click += new RoutedEventHandler(CreateBadgeAndTextTile_Click);
        }

        private async void CreateBadgeTile_Click(object sender, RoutedEventArgs e)
        {
            if (!SecondaryTile.Exists(BADGE_TILE_ID)) {
                SecondaryTile secondTile = new SecondaryTile(
                    BADGE_TILE_ID,
                    "LockScreen CS - Badge only",
                    "BADGE_ARGS",
                    new Uri("ms-appx:///images/squareTile-sdk.png"),
                    TileSize.Square150x150
                );
                secondTile.LockScreenBadgeLogo = new Uri("ms-appx:///images/badgelogo-sdk.png");

                bool isPinned = await secondTile.RequestCreateForSelectionAsync(GetElementRect((FrameworkElement) sender), Placement.Above);
                if (isPinned)
                {
                    BadgeNumericNotificationContent badgeContent = new BadgeNumericNotificationContent(2);
                    BadgeUpdateManager.CreateBadgeUpdaterForSecondaryTile(BADGE_TILE_ID).Update(badgeContent.CreateNotification());
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
                    new Uri("ms-appx:///images/squareTile-sdk.png"),
                    TileSize.Wide310x150
                );
                secondTile.VisualElements.Wide310x150Logo = new Uri("ms-appx:///images/tile-sdk.png");
                secondTile.LockScreenBadgeLogo = new Uri("ms-appx:///images/badgelogo-sdk.png");
                secondTile.LockScreenDisplayBadgeAndTileText = true;

                bool isPinned = await secondTile.RequestCreateForSelectionAsync(GetElementRect((FrameworkElement) sender), Placement.Above);
                if (isPinned)
                {
                    ITileWide310x150Text03 tileContent = TileContentFactory.CreateTileWide310x150Text03();
                    tileContent.TextHeadingWrap.Text = "Text for the lock screen";
                    tileContent.RequireSquare150x150Content = false;
                    TileUpdateManager.CreateTileUpdaterForSecondaryTile(TEXT_TILE_ID).Update(tileContent.CreateNotification());
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
            Point point = buttonTransform.TransformPoint(new Point());
            return new Rect(point, new Size(element.ActualWidth, element.ActualHeight));
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Get a pointer to our main page
            rootPage = e.Parameter as MainPage;
        }
    }
}
