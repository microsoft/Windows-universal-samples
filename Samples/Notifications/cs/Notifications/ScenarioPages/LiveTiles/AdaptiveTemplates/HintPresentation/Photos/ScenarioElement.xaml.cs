using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.AdaptiveTemplates.HintPresentation.Photos
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
            updateFourTilesControl.SetTilePayload($@"
                <tile>
                    <visual>

                        <binding template='TileMedium' hint-presentation='photos'>
                            <image src='Assets/Photos/1.jpg'/>
                            <image src='Assets/Photos/2.jpg'/>
                            <image src='Assets/Photos/3.jpg'/>
                            <image src='Assets/Photos/4.jpg'/>
                            <image src='Assets/Photos/5.jpg'/>
                            <image src='Assets/Photos/6.jpg'/>
                            <image src='Assets/Photos/7.jpg'/>
                            <image src='Assets/Photos/8.jpg'/>
                            <image src='Assets/Photos/9.jpg'/>
                            <image src='Assets/Photos/10.jpg'/>
                        </binding>

                        <binding template='TileWide' hint-presentation='photos'>
                            <image src='Assets/Photos/1.jpg'/>
                            <image src='Assets/Photos/2.jpg'/>
                            <image src='Assets/Photos/3.jpg'/>
                            <image src='Assets/Photos/4.jpg'/>
                            <image src='Assets/Photos/5.jpg'/>
                            <image src='Assets/Photos/6.jpg'/>
                            <image src='Assets/Photos/7.jpg'/>
                            <image src='Assets/Photos/8.jpg'/>
                            <image src='Assets/Photos/9.jpg'/>
                            <image src='Assets/Photos/10.jpg'/>
                        </binding>

                        <binding template='TileLarge' hint-presentation='photos'>
                            <image src='Assets/Photos/1.jpg'/>
                            <image src='Assets/Photos/2.jpg'/>
                            <image src='Assets/Photos/3.jpg'/>
                            <image src='Assets/Photos/4.jpg'/>
                            <image src='Assets/Photos/5.jpg'/>
                            <image src='Assets/Photos/6.jpg'/>
                            <image src='Assets/Photos/7.jpg'/>
                            <image src='Assets/Photos/8.jpg'/>
                            <image src='Assets/Photos/9.jpg'/>
                            <image src='Assets/Photos/10.jpg'/>
                        </binding>

                    </visual>
                </tile>");
        }
    }
}
