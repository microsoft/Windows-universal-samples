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
using System;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
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
    public sealed partial class PinTileAndUpdateOnSuspend : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        AppBar appBar;

        public PinTileAndUpdateOnSuspend()
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

        const string SCENARIO2_TILEID = "Scenario2.Tile";

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
        /// This is the click handler for the 'Pin tile and update on suspend' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void PinTileAndUpdateOnSuspend_Click(object sender, RoutedEventArgs e)
        {
            App.OnNewTilePinned = UpdateTile;

            // Create the original Square150x150 tile. The image to display on the tile has a purple background and the word "Original" in white text.
            var tile = new SecondaryTile(MainPage.logoSecondaryTileId, "Scenario 9", "/MainPage.xaml?scenario=Scenario9", new Uri("ms-appx:///Assets/square150x150Tile-sdk.png"), TileSize.Default);
            tile.VisualElements.ShowNameOnSquare150x150Logo = true;
            await tile.RequestCreateAsync();

            // When a new tile is created, the app will be suspended and the new tile will be displayed to the user on the start screen.
            // Any code after the call to RequestCreateAsync is not guaranteed to run. 
            // For example, a common scenario is to associate a push channel with the newly created tile,
            // which involves a call to WNS to get a channel using the CreatePushNotificationChannelForSecondaryTileAsync() asynchronous operation. 
            // Another example is updating the secondary tile with data from a web service. Both of these are examples of actions that may not
            // complete before the app is suspended. To illustrate this, we'll create a delay and then attempt to update our secondary tile.
        }

        /// <summary>
        /// Delegate to update the secondary tile. This is called from the OnSuspending event handler in App.xaml.cs
        /// </summary>
        private void UpdateTile()
        {
            // Simulate a long-running task. For illustration purposes only. 
            if (Debugger.IsAttached)
            {
                // Set a larger delay to give you time to select "Suspend" from the "LifetimeEvents" dropdown in Visual Studio in 
                // order to simulate the app being suspended when the new tile is created. 
                Task.Delay(8000).Wait();
            }
            else
            {
                // When the app is not attached to the debugger, the app will be suspended so we can use a 
                // more realistic delay.
                Task.Delay(2000).Wait();
            }

            // Update the tile we created using a notification.
            var tileXml = TileUpdateManager.GetTemplateContent(TileTemplateType.TileSquare150x150Image);

            // The TileSquare150x150Image template only contains one image entry, so retrieve it.
            var imageElement = tileXml.GetElementsByTagName("image").Single();

            // Set the src propertry on the image entry. The image in this sample is a lime green image with the word "Updated" in white text 
            imageElement.Attributes.GetNamedItem("src").NodeValue = "ms-appx:///Assets/alternate1Square150x150Tile-sdk.png";

            // Create a new tile notification.
            var notification = new Windows.UI.Notifications.TileNotification(tileXml);

            // Create a tile updater.
            var updater = TileUpdateManager.CreateTileUpdaterForSecondaryTile(MainPage.logoSecondaryTileId);

            // Send the update notification for the tile. 
            updater.Update(notification);
        }
    }
}
