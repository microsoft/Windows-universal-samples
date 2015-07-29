using System;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.ActivationTypes.System.Dismiss.Dismissing
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

            // Pop reminder
            popToastControl.Payload =
                $@"
                <toast scenario='reminder' launch='args'>
                    <visual>
                        <binding template='ToastGeneric'>
                            <text>System Dismiss (Dismissing)</text>
                            <text>Minimize the app/return to Start, click ""dismiss"", and then manually return to the app to continue to the next step.</text>
                        </binding>
                    </visual>

                    <actions>

                        <action activationType='system' arguments='dismiss' content='dismiss' />

                    </actions>
                </toast>";
        }

        private async void Error(string message)
        {
            await new MessageDialog(message, "ERROR").ShowAsync();
        }
    }
}
