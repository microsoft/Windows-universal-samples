using System;
using System.Linq;
using System.Threading.Tasks;
using Windows.Data.Xml.Dom;
using Windows.UI;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Badges.Local.SecondaryTileBadges
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ScenarioElement : UserControl
    {
        private static readonly string SECONDARY_TILE_ID = "badge";
        
        private void ButtonClearBadge_Click(object sender, RoutedEventArgs e)
        {
            // Clear the badge from the secondary tile
            BadgeUpdateManager.CreateBadgeUpdaterForSecondaryTile(SECONDARY_TILE_ID).Clear();
        }

        private async void ButtonUpdateBadgeNumber_Click(object sender, RoutedEventArgs e)
        {
            int num;

            if (!int.TryParse(TextBoxBadgeNumber.Text, out num))
            {
                await new MessageDialog("You must provide a valid integer.", "Error").ShowAsync();
                return;
            }

            // Get the blank badge XML payload for a badge number
            XmlDocument badgeXml = BadgeUpdateManager.GetTemplateContent(BadgeTemplateType.BadgeNumber);

            // Set the value of the badge in the XML to our number
            XmlElement badgeElement = badgeXml.SelectSingleNode("/badge") as XmlElement;
            badgeElement.SetAttribute("value", num.ToString());

            // Create the badge notification
            BadgeNotification badge = new BadgeNotification(badgeXml);

            // Create the badge updater for our secondary tile, using our tile ID for the secondary tile
            BadgeUpdater badgeUpdater = BadgeUpdateManager.CreateBadgeUpdaterForSecondaryTile(SECONDARY_TILE_ID);

            // And update the badge
            badgeUpdater.Update(badge);
        }

        private void ButtonUpdateBadgeGlyph_Click(object sender, RoutedEventArgs e)
        {
            string badgeGlyphValue = ComboBoxBadgeGlyph.SelectedItem as string;

            // Get the blank badge XML payload for a badge glyph
            XmlDocument badgeXml = BadgeUpdateManager.GetTemplateContent(BadgeTemplateType.BadgeGlyph);

            // Set the value of the badge in the XML to our glyph value
            XmlElement badgeElement = badgeXml.SelectSingleNode("/badge") as XmlElement;
            badgeElement.SetAttribute("value", badgeGlyphValue);

            // Create the badge notification
            BadgeNotification badge = new BadgeNotification(badgeXml);

            // Create the badge updater for our secondary tile, using our tile ID for the secondary tile
            BadgeUpdater badgeUpdater = BadgeUpdateManager.CreateBadgeUpdaterForSecondaryTile(SECONDARY_TILE_ID);

            // And update the badge
            badgeUpdater.Update(badge);
        }


        


        public ScenarioElement()
        {
            this.InitializeComponent();

            this.InitializeScenario();
        }

        private async void InitializeScenario()
        {
            base.IsEnabled = false;

            // If secondary tile already exists, we can continue to the next step automatically
            if (await DoesSecondaryTileExist())
                MoveToBadgeStep();
            else
                MoveToSecondaryTileStep();

            base.IsEnabled = true;
        }

        private async Task<bool> DoesSecondaryTileExist()
        {
            var secondaryTiles = await SecondaryTile.FindAllAsync();

            return secondaryTiles.Any(i => i.TileId.Equals(SECONDARY_TILE_ID));
        }

        private void MoveToBadgeStep()
        {
            ScrollViewerSecondaryTile.Visibility = Visibility.Collapsed;
            ScrollViewerBadge.Visibility = Visibility.Visible;
        }

        private void MoveToSecondaryTileStep()
        {
            ScrollViewerSecondaryTile.Visibility = Visibility.Visible;
            ScrollViewerBadge.Visibility = Visibility.Collapsed;
        }

        private async void ButtonPinSecondaryTile_Click(object sender, RoutedEventArgs e)
        {
            // Create and pin new secondary tile for badges
            SecondaryTile tile = new SecondaryTile(SECONDARY_TILE_ID, "Badges", "args", new Uri("ms-appx:///Assets/Logo.png"), TileSize.Default);
            tile.VisualElements.ShowNameOnSquare150x150Logo = true;
            tile.VisualElements.ShowNameOnSquare310x310Logo = true;
            tile.VisualElements.ShowNameOnWide310x150Logo = true;
            tile.VisualElements.BackgroundColor = Colors.Blue;
            await tile.RequestCreateAsync();

            // Move to the next step
            MoveToBadgeStep();
        }
    }
}
