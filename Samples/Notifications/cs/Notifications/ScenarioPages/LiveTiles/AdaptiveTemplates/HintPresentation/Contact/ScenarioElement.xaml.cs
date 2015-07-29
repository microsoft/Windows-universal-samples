using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.AdaptiveTemplates.HintPresentation.Contact
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

                        <binding template='TileSmall' hint-presentation='contact'>
                            <image src='Assets/Photos/1.jpg'/>
                        </binding>

                        <binding template='TileMedium' hint-presentation='contact'>
                            <image src='Assets/Photos/1.jpg'/>
                            <image src='Assets/Photos/2.jpg'/>
                            <text>CONTACT_TEXT01</text>
                            <text>CONTACT_TEXT02</text>
                            <text>CONTACT_TEXT03</text>
                        </binding>

                        <binding template='TileWide' hint-presentation='contact'>
                            <image src='Assets/Photos/1.jpg'/>
                            <image src='Assets/Photos/2.jpg'/>
                            <text>CONTACT_TEXT01</text>
                            <text>CONTACT_TEXT02</text>
                            <text>CONTACT_TEXT03</text>
                        </binding>

                        <binding template='TileLarge' hint-presentation='contact'>
                            <image src='Assets/Photos/1.jpg'/>
                            <image src='Assets/Photos/2.jpg'/>
                            <text>CONTACT_TEXT01</text>
                            <text>CONTACT_TEXT02</text>
                            <text>CONTACT_TEXT03</text>
                        </binding>

                    </visual>
                </tile>");
        }
    }
}
