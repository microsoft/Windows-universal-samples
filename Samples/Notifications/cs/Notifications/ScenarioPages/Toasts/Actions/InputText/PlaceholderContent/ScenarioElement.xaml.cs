using System;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.Actions.InputText.PlaceholderContent
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
                <toast activationType='foreground' launch='args'>
                    <visual>
                        <binding template='ToastGeneric'>
                            <text>Input Text - Placeholder</text>
                            <text>Make sure ""My placeholder content"" is displayed as the placeholder content of the text box in this toast.</text>
                        </binding>
                    </visual>
                    <actions>

                        <input
                            id = 'message'
                            type = 'text'
                            placeHolderContent = 'My placeholder content' />

                    </actions>
                </toast>";
        }

        private async void Error(string message)
        {
            await new MessageDialog(message, "ERROR").ShowAsync();
        }
    }
}
