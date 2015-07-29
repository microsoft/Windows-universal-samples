using System;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.AdaptiveTemplates.Image.Src.FromInternet
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

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            Initialize();
        }

        private void Initialize()
        {
            // Clear all existing notifications
            ToastNotificationManager.History.Clear();

            // Pop notifications
            popToastControl.Payload =
                $@"
                <toast scenario='reminder'>
                    <visual>
                        <binding template='ToastGeneric'>
                            <text>Image Src - From Internet</text>
                            <text>Make sure an image is displayed below this text.</text>
                            <image src='http://hiking.andrewbares.net/trips/2/images/large/p193jj6k4e58c1c7k1486u8n6vt6.jpg'/>
                        </binding>
                    </visual>
                </toast>";
        }

        private async void Error(string message)
        {
            await new MessageDialog(message, "ERROR").ShowAsync();
        }
    }
}
