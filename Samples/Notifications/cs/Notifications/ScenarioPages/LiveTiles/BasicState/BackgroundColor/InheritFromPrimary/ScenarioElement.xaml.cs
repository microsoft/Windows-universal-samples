using System;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.BasicState.BackgroundColor.InheritFromPrimary
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

            SecondaryTile tile = new SecondaryTile(_tileId, "Inherit Primary Color", "args", new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Medium.png"), TileSize.Default);
            tile.VisualElements.Square71x71Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Small.png");
            tile.VisualElements.Wide310x150Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Wide.png");
            tile.VisualElements.Square310x310Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Large.png");
            tile.VisualElements.ShowNameOnSquare150x150Logo = true;
            tile.VisualElements.ShowNameOnSquare310x310Logo = true;
            tile.VisualElements.ShowNameOnWide310x150Logo = true;
            await tile.RequestCreateAsync();
            
            base.IsEnabled = true;
        }
    }
}
