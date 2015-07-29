using Notifications.Helpers;
using System;
using System.Linq;
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.Notifications.SecondaryTiles
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

        private void ButtonSendNotification_Click(object sender, RoutedEventArgs e)
        {
            string xml = $@"
                <tile version='3'>
                    <visual branding='nameAndLogo'>

                        <binding template='TileMedium'>
                            <text hint-wrap='true'>New tile notification</text>
                            <text hint-wrap='true' hint-style='captionSubtle'/>
                        </binding>

                        <binding template='TileWide'>
                            <text hint-wrap='true'>New tile notification</text>
                            <text hint-wrap='true' hint-style='captionSubtle'/>
                        </binding>

                        <binding template='TileLarge'>
                            <text hint-wrap='true'>New tile notification</text>
                            <text hint-wrap='true' hint-style='captionSubtle'/>
                        </binding>

                </visual>
            </tile>";

            XmlDocument doc = new XmlDocument();
            doc.LoadXml(xml);

            string nowTimeString = DateTime.Now.ToString();

            // Assign date/time values through XmlDocument to avoid any xml escaping issues
            foreach (XmlElement textEl in doc.SelectNodes("//text").OfType<XmlElement>())
                if (textEl.InnerText.Length == 0)
                    textEl.InnerText = nowTimeString;

            TileNotification notification = new TileNotification(doc);
            TileUpdateManager.CreateTileUpdaterForSecondaryTile(_tileId).Update(notification);
        }

        private async void ButtonPinTile_Click(object sender, RoutedEventArgs e)
        {
            base.IsEnabled = false;

            _tileId = DateTime.Now.Ticks.ToString();

            SecondaryTile tile = TilesHelper.GenerateSecondaryTile(_tileId, "Secondary notification");
            tile.VisualElements.ShowNameOnSquare150x150Logo = true;
            tile.VisualElements.ShowNameOnSquare310x310Logo = true;
            tile.VisualElements.ShowNameOnWide310x150Logo = true;
            await tile.RequestCreateAsync();

            stepsControl.Step++;
            base.IsEnabled = true;
        }
    }
}
