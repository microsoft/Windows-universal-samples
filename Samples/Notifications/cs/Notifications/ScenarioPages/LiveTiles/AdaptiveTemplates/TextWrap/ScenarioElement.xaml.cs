using Notifications.Helpers;
using System;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.AdaptiveTemplates.TextWrap
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

            string text = "Now, this is the story all about how my tile got text-wrapped upside down. And I'd like to take a minute, just sit right there, I'll tell you how I became the prince of a town called Bellevue. In South Arizona, born and raised, in the mountains is where I spent most of my days, climbin' up, runnin', posin' all cool, and all shootin' some videos outside of school. When a couple of guys who were up to no good, started leavin' trash at the trailhead.";

            await TilesHelper.PinNewSecondaryTile("text wrapping",
                $@"
                <tile version='3'>
                    <visual>

                        <binding template='TileSmall'>
                            <text hint-wrap='true'>{text}</text>
                        </binding>

                        <binding template='TileMedium'>
                            <text hint-wrap='true'>{text}</text>
                        </binding>

                        <binding template='TileWide'>
                            <text hint-wrap='true'>{text}</text>
                        </binding>

                        <binding template='TileLarge'>
                            <text hint-wrap='true'>{text}</text>
                        </binding>

                </visual>
            </tile>");


            await new MessageDialog("SUCCESS").ShowAsync();
            ButtonPin.IsEnabled = true;
        }
    }
}
