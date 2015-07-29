using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.AdaptiveTemplates.Branding.None
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ScenarioElement : UserControl
    {
        public ScenarioElement()
        {
            this.InitializeComponent();

            // Because different densities might be able to display more, we just set tons of text wrapped to make sure it overflows at some point
            string fillerText = "<text hint-wrap='true'>bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb</text>";

            updateFourTilesControl.SetTilePayload($@"
                <tile version='3'>
                    <visual>

                        <binding template='TileSmall' branding='none'>
                            <text>none</text>
                            {fillerText}
                        </binding>

                        <binding template='TileMedium' branding='none'>
                            <text hint-wrap='true'>branding: none</text>
                            {fillerText}
                        </binding>

                        <binding template='TileWide' branding='none'>
                            <text hint-wrap='true'>branding: none</text>
                            {fillerText}
                        </binding>

                        <binding template='TileLarge' branding='none'>
                            <text hint-wrap='true'>branding: none</text>
                            {fillerText}
                        </binding>

                    </visual>
                </tile>");
        }
    }
}
