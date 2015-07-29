using Notifications.Helpers;
using System;
using System.Threading.Tasks;
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.AdaptiveTemplates.TextStyles
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

        private async void ButtonPin_Click(object sender, RoutedEventArgs e)
        {
            ButtonPin.IsEnabled = false;
            
            await PinTile("caption");
            await PinTile("body");
            await PinTile("base");
            await PinTile("subtitle");
            await PinTile("title");
            await PinTile("subheader");
            await PinTile("header");
            await PinTile("captionsubtle");
            await PinTile("bodysubtle");
            await PinTile("basesubtle");
            await PinTile("subtitlesubtle");
            await PinTile("titlesubtle");
            await PinTile("subheadersubtle");
            await PinTile("headersubtle");
            await PinTile("subheadernumeral");
            await PinTile("headernumeral");


            await new MessageDialog("SUCCESS").ShowAsync();
            ButtonPin.IsEnabled = true;
        }

        private async Task PinTile(string style)
        {
            SecondaryTile tile = TilesHelper.GenerateSecondaryTile(style);
            tile.VisualElements.ShowNameOnSquare150x150Logo = true;
            tile.VisualElements.ShowNameOnSquare310x310Logo = true;
            tile.VisualElements.ShowNameOnWide310x150Logo = true;

            await tile.RequestCreateAsync();

            string xml = $@"
                <tile version='3'>
                    <visual>
       
                        <binding template='TileSmall'>
                            <text hint-style='{style}'>This is a string of text that will be styled to the desired effect</text>
                        </binding>

                        <binding template='TileMedium'>
                            <text hint-style='{style}'>This is a string of text that will be styled to the desired effect</text>
                        </binding>

                        <binding template='TileWide'>
                            <text hint-style='{style}'>This is a string of text that will be styled to the desired effect</text>
                        </binding>

                        <binding template='TileLarge'>
                            <text hint-style='{style}'>This is a string of text that will be styled to the desired effect</text>
                        </binding>
                    </visual>
                </tile>";

            XmlDocument doc = new XmlDocument();
            doc.LoadXml(xml);

            TileNotification notification = new TileNotification(doc);
            TileUpdateManager.CreateTileUpdaterForSecondaryTile(tile.TileId).Update(notification);
        }
    }
}
