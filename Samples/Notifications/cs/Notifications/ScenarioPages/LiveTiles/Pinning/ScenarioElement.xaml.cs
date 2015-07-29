using System;
using Windows.UI.Popups;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.Pinning
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

            try
            {
                SecondaryTile tile = new SecondaryTile(DateTime.Now.Ticks.ToString());
                tile.Arguments = "args";

                tile.DisplayName = TextBoxDisplayName.Text;

                if (CheckBoxSquare150x150Logo.IsChecked.Value)
                    tile.VisualElements.Square150x150Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Medium.png");

                if (CheckBoxSquare71x71Logo.IsChecked.Value)
                    tile.VisualElements.Square71x71Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Small.png");

                if (CheckBoxWide310x150Logo.IsChecked.Value)
                    tile.VisualElements.Wide310x150Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Wide.png");

                if (CheckBoxSquare310x310Logo.IsChecked.Value)
                    tile.VisualElements.Square310x310Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Large.png");

                tile.VisualElements.ShowNameOnSquare150x150Logo = CheckBoxShowNameOnSquare150x150Logo.IsChecked.Value;
                tile.VisualElements.ShowNameOnSquare310x310Logo = CheckBoxShowNameOnSquare310x310Logo.IsChecked.Value;
                tile.VisualElements.ShowNameOnWide310x150Logo = CheckBoxShowNameOnWide310x150Logo.IsChecked.Value;
                
                await tile.RequestCreateAsync();
            }

            catch (Exception ex)
            {
                await new MessageDialog(ex.ToString(), "Error").ShowAsync();
            }

            ButtonPin.IsEnabled = true;
        }
    }
}
