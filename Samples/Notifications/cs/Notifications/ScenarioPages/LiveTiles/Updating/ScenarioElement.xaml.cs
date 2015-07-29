using System;
using System.Linq;
using Windows.UI.Popups;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.Updating
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

            // Generate a unique tile ID for the purposes of the sample
            _tileId = DateTime.Now.Ticks.ToString();

            // Initialize and pin a new secondary tile that we will later update
            SecondaryTile tile = new SecondaryTile(_tileId, "Original", "args", new Uri("ms-appx:///Assets/cancel.png"), TileSize.Default);
            tile.VisualElements.Square71x71Logo = new Uri("ms-appx:///Assets/cancel.png");
            tile.VisualElements.Wide310x150Logo = new Uri("ms-appx:///Assets/cancel.png");
            tile.VisualElements.Square310x310Logo = new Uri("ms-appx:///Assets/cancel.png");
            tile.VisualElements.ShowNameOnSquare150x150Logo = true;
            tile.VisualElements.ShowNameOnSquare310x310Logo = true;
            tile.VisualElements.ShowNameOnWide310x150Logo = true;
            await tile.RequestCreateAsync();

            // And move to the next step
            stepsControl.Step++;
            
            base.IsEnabled = true;
        }

        private async void ButtonUpdateTile_Click(object sender, RoutedEventArgs e)
        {
            base.IsEnabled = false;
            
            // Grab the existing tile
            SecondaryTile tile = (await SecondaryTile.FindAllAsync()).FirstOrDefault(i => i.TileId.Equals(_tileId));
            if (tile == null)
            {
                await new MessageDialog("The secondary tile that was previously pinned could not be found. Has it been removed from Start?", "Error").ShowAsync();
                return;
            }

            // Change its name and logos
            tile.DisplayName = "Updated";
            tile.VisualElements.Square150x150Logo = new Uri("ms-appx:///Assets/check.png");
            tile.VisualElements.Square71x71Logo = new Uri("ms-appx:///Assets/check.png");
            tile.VisualElements.Square310x310Logo = new Uri("ms-appx:///Assets/check.png");
            tile.VisualElements.Wide310x150Logo = new Uri("ms-appx:///Assets/check.png");

            // And request its properties to be updated
            await tile.UpdateAsync();

            stepsControl.Step++;

            base.IsEnabled = true;
        }

        private void stepsControl_StepChanged(object sender, int e)
        {
            switch (e)
            {
                case 1:
                    stepsControl.IsNextEnabled = true;
                    break;

                case 2:
                    stepsControl.IsNextEnabled = false;
                    break;

                case 3:
                    stepsControl.IsNextEnabled = true;
                    break;

                case 4:
                    stepsControl.IsNextEnabled = false;
                    break;

                default:
                    stepsControl.IsNextEnabled = true;
                    break;
            }
        }
    }
}
