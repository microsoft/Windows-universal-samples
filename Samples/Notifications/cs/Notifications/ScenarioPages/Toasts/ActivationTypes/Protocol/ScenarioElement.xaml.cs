using System;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.ActivationTypes.Protocol
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
                <toast activationType='protocol' launch='bingmaps:?q=1+Microsoft+Way,+98052'>
                    <visual>
                        <binding template='ToastGeneric'>
                            <text>Click to launch Bing Maps</text>
                            <text>Searches for ""1 Microsoft Way, 98052""</text>
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
