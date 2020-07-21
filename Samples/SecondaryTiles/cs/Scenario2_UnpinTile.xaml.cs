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
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SecondaryTiles
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class UnpinTile : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        AppBar appBar;

        public UnpinTile()
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
        /// This is the click handler for the 'Unpin' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void UnpinSecondaryTile_Click(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;
            if (button != null)
            {
                if (Windows.UI.StartScreen.SecondaryTile.Exists(MainPage.logoSecondaryTileId))
                {
                    // First prepare the tile to be unpinned
                    SecondaryTile secondaryTile = new SecondaryTile(MainPage.logoSecondaryTileId);
                    // Now make the delete request.
                    bool isUnpinned = await secondaryTile.RequestDeleteForSelectionAsync(MainPage.GetElementRect((FrameworkElement)sender), Windows.UI.Popups.Placement.Below);
                    if (isUnpinned)
                    {
                        rootPage.NotifyUser("Secondary tile successfully unpinned.", NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser("Secondary tile not unpinned.", NotifyType.ErrorMessage);
                    }
                }
                else
                {
                    rootPage.NotifyUser(MainPage.logoSecondaryTileId + " is not currently pinned.", NotifyType.ErrorMessage);
                }
            }
        }
    }
}
