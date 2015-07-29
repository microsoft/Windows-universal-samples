using Notifications.Helpers;
using System;
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.AdaptiveTemplates.Images.Source.FromAppPackage
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

            string ImageUrl = "ms-appx:///Assets/check.png";

            SecondaryTile tile = TilesHelper.GenerateSecondaryTile("App Package Image");
            await tile.RequestCreateAsync();

            string tileXmlString =
                "<tile>"
                + "<visual>"
                + "<binding template='TileSmall'>"
                + "<image src='" + ImageUrl + "' alt='image'/>"
                + "</binding>"
                + "<binding template='TileMedium'>"
                + "<image src='" + ImageUrl + "' alt='image'/>"
                + "</binding>"
                + "<binding template='TileWide'>"
                + "<image src='" + ImageUrl + "' alt='image'/>"
                + "</binding>"
                + "<binding template='TileLarge'>"
                + "<image src='" + ImageUrl + "' alt='image'/>"
                + "</binding>"
                + "</visual>"
                + "</tile>";

            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.LoadXml(tileXmlString);
            TileNotification notifyTile = new TileNotification(xmlDoc);
            TileUpdateManager.CreateTileUpdaterForSecondaryTile(tile.TileId).Update(notifyTile);
            
            base.IsEnabled = true;
        }
    }
}
