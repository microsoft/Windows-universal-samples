using System;
using Windows.UI;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.FindAllAsync.Visualizer
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ScenarioElement : UserControl
    {
        public ScenarioElement()
        {
            this.InitializeComponent();

            ButtonUpdate_Click(this, null);
        }

        private async void ButtonUpdate_Click(object sender, RoutedEventArgs e)
        {
            ListViewTiles.ItemsSource = await SecondaryTile.FindAllAsync();
        }

        private void blah()
        {
            SecondaryTile tile = new SecondaryTile();
            tile.DisplayName = "";
            tile.VisualElements.BackgroundColor = Colors.Red;
        }
    }
}
