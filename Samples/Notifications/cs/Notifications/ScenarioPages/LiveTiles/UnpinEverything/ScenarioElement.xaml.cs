using System;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.UnpinEverything
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

        private async void ButtonUnpinEverything_Click(object sender, RoutedEventArgs e)
        {
            ButtonUnpinEverything.IsEnabled = false;

            // Loop through every secondary tile
            foreach (SecondaryTile tile in await SecondaryTile.FindAllAsync())
            {
                // Unpin each secondary tile
                await tile.RequestDeleteAsync();
            }

            ButtonUnpinEverything.IsEnabled = true;
        }
    }
}
