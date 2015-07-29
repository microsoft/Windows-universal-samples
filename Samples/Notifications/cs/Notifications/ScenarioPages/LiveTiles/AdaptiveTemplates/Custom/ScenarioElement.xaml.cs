using Notifications.Helpers;
using System;
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.AdaptiveTemplates.Custom
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

        private async void ButtonUpdate_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                XmlDocument doc = new XmlDocument();
                doc.LoadXml(TextBoxTilePayload.Text);

                Update("CustomSmall", doc);
                Update("CustomMedium", doc);
                Update("CustomWide", doc);
                Update("CustomLarge", doc);
            }

            catch (Exception ex)
            {
                await new MessageDialog(ex.ToString(), "ERROR").ShowAsync();
            }
        }

        private void Update(string tileId, XmlDocument doc)
        {
            if (SecondaryTile.Exists(tileId))
                TileUpdateManager.CreateTileUpdaterForSecondaryTile(tileId).Update(new TileNotification(doc));
        }

        private async void ButtonPin_Click(object sender, RoutedEventArgs e)
        {
            await TilesHelper.GenerateSecondaryTile("CustomSmall", "Small").RequestCreateAsync();
            await TilesHelper.GenerateSecondaryTile("CustomMedium", "Medium").RequestCreateAsync();
            await TilesHelper.GenerateSecondaryTile("CustomWide", "Wide").RequestCreateAsync();
            await TilesHelper.GenerateSecondaryTile("CustomLarge", "Large").RequestCreateAsync();
        }
    }
}
