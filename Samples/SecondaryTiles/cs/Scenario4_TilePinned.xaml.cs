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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SecondaryTiles
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class TilePinned : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        AppBar appBar;

        public TilePinned()
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
        /// This is the click handler for the 'TileExists' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void TileExists_Click(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;
            if (button != null)
            {
                if (Windows.UI.StartScreen.SecondaryTile.Exists(MainPage.logoSecondaryTileId))
                {
                    rootPage.NotifyUser(MainPage.logoSecondaryTileId + " is pinned.", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser(MainPage.logoSecondaryTileId + " not currently pinned.", NotifyType.ErrorMessage);
                }
            }
        }
    }
}
