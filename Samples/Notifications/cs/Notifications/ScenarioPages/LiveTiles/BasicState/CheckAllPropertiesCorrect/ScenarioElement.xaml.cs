using OneNotifications_UWP.Helpers;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI;
using Windows.UI.Popups;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace OneNotifications_UWP.ScenarioPages.LiveTiles.BasicState.CheckAllPropertiesCorrect
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

            SecondaryTile tile = new SecondaryTile(_tileId, "Transparent", "args", new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Medium.png"), TileSize.Default);
            tile.VisualElements.Square71x71Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Small.png");
            tile.VisualElements.Wide310x150Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Wide.png");
            tile.VisualElements.Square310x310Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Large.png");
            tile.VisualElements.ShowNameOnSquare150x150Logo = true;
            tile.VisualElements.ShowNameOnSquare310x310Logo = true;
            tile.VisualElements.ShowNameOnWide310x150Logo = true;
            tile.VisualElements.BackgroundColor = Colors.Transparent;
            await tile.RequestCreateAsync();
            
            base.IsEnabled = true;
        }
    }
}
