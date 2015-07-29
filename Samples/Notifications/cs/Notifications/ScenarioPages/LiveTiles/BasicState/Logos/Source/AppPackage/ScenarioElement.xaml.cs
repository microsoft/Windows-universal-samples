using System;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.BasicState.Logos.Source.AppPackage
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

        private async void ButtonPinTile_Click(object sender, RoutedEventArgs e)
        {
            base.IsEnabled = false;

            string tileId = DateTime.Now.Ticks.ToString();

            SecondaryTile tile = new SecondaryTile(tileId, "Logo App Package", "args", new Uri("ms-appx:///Assets/Photos/1.jpg"), TileSize.Default);
            await tile.RequestCreateAsync();

            base.IsEnabled = true;
        }
    }
}
