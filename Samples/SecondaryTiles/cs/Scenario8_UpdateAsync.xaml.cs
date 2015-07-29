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
    public sealed partial class UpdateAsync : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        AppBar appBar;

        public UpdateAsync()
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
        /// This is the click handler for the 'Update logo async' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void UpdateDefaultLogo_Click(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;
            if (button != null)
            {
                if (Windows.UI.StartScreen.SecondaryTile.Exists(MainPage.logoSecondaryTileId))
                {
                    // Add the properties we want to update (logo in this example)
                    SecondaryTile secondaryTile = new SecondaryTile(MainPage.logoSecondaryTileId);
                    secondaryTile.VisualElements.Square150x150Logo = new Uri("ms-appx:///Assets/squareTileLogoUpdate-sdk.png");
                    bool isUpdated = await secondaryTile.UpdateAsync();

                    if (isUpdated)
                    {
                        rootPage.NotifyUser("Secondary tile logo updated.", NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser("Secondary tile logo not updated.", NotifyType.ErrorMessage);
                    }
                }
                else
                {
                    rootPage.NotifyUser("Please pin a secondary tile using scenario 1 before updating the Logo.", NotifyType.ErrorMessage);
                }
            }
        }
    }
}
