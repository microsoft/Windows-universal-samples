using System;
using System.Linq;
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.Notifications.ExpiringNotification
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ScenarioElement : UserControl
    {
        public ScenarioElement()
        {
            this.InitializeComponent();
        }

        private string _tileId;

        private async void ButtonPinSecondaryTile_Click(object sender, RoutedEventArgs e)
        {
            base.IsEnabled = false;

            _tileId = DateTime.Now.Ticks.ToString();

            SecondaryTile tile = new SecondaryTile(_tileId, "Expiring Notification", "args", new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Medium.png"), TileSize.Default);
            tile.VisualElements.Square71x71Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Small.png");
            tile.VisualElements.Wide310x150Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Wide.png");
            tile.VisualElements.Square310x310Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Large.png");
            tile.VisualElements.ShowNameOnSquare150x150Logo = true;
            tile.VisualElements.ShowNameOnSquare310x310Logo = true;
            tile.VisualElements.ShowNameOnWide310x150Logo = true;
            await tile.RequestCreateAsync();
            
            base.IsEnabled = true;
        }

        private async void ButtonSendNotification_Click(object sender, RoutedEventArgs e)
        {
            base.IsEnabled = false;

            try
            {
                if (_tileId == null)
                {
                    await new MessageDialog("No secondary tile was pinned. In the previous step, you had to pin the tile.", "Error").ShowAsync();
                    return;
                }

                SecondaryTile tile = (await SecondaryTile.FindAllAsync()).FirstOrDefault(i => i.TileId.Equals(_tileId));
                if (tile == null)
                {
                    await new MessageDialog("The secondary tile that was previously pinned could not be found. Has it been removed from Start?", "Error").ShowAsync();
                    return;
                }

                // Decide expiration time
                Windows.Globalization.Calendar cal = new Windows.Globalization.Calendar();
                cal.SetToNow();
                cal.AddSeconds(20);

                // Get expiration time and date
                var longTime = new Windows.Globalization.DateTimeFormatting.DateTimeFormatter("longtime");
                DateTimeOffset expiryTime = cal.GetDateTime();
                string expiryTimeString = longTime.Format(expiryTime);

                // Create the custom tile that will expire
                string tileXmlString = 
                "<tile>"
                + "<visual>"
                + "<binding template='TileMedium'>"
                + "<text hint-wrap='true'>This notification will expire at " + expiryTimeString + "</text>"
                + "</binding>"
                + "<binding template='TileWide'>"
                + "<text hint-wrap='true'>This notification will expire at " + expiryTimeString + "</text>"
                + "</binding>"
                + "<binding template='TileLarge'>"
                + "<text hint-wrap='true'>This notification will expire at " + expiryTimeString + "</text>"
                + "</binding>"
                + "</visual>"
                + "</tile>";

                XmlDocument xmlDoc = new XmlDocument();
                xmlDoc.LoadXml(tileXmlString);

                // Create the notification
                TileNotification notifyTile = new TileNotification(xmlDoc);

                // Set expiration time for the notification
                notifyTile.ExpirationTime = expiryTime;

                // And send the notification to the tile
                TileUpdateManager.CreateTileUpdaterForSecondaryTile(tile.TileId).Update(notifyTile);
            }

            catch (Exception ex)
            {
                await new MessageDialog(ex.ToString(), "Error updating tile").ShowAsync();
            }

            finally
            {
                base.IsEnabled = true;
            }
        }
    }
}
