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
using System.Collections.Generic;
using System.Text;
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
    public sealed partial class EnumerateTiles : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        AppBar appBar;

        public EnumerateTiles()
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
        /// This is the click handler for the 'Enumerate tile' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void EnumerateSecondaryTiles_Click(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;
            if (button != null)
            {
                // Get secondary tile ids for this package
                IReadOnlyList<SecondaryTile> tilelist = await Windows.UI.StartScreen.SecondaryTile.FindAllAsync();
                if (tilelist.Count > 0)
                {
                    int count = 0;
                    StringBuilder outputText = new StringBuilder();
                    foreach (var tile in tilelist)
                    {
                        outputText.AppendFormat("Tile Id[{0}] = {1}, Tile short display name = {2}  {3}", count++, tile.TileId, tile.DisplayName, System.Environment.NewLine);
                    }
                    rootPage.NotifyUser(outputText.ToString(), NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("No secondary tiles are available for this appId.", NotifyType.ErrorMessage);
                }
            }
        }
    }
}
