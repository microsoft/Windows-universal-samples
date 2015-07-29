using System;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.Actions.Action.TextAndImage
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
                            <text>Action - text and image</text>
                            <text>Make sure the left button has a ""check"" icon with the text ""check"" below it, and the right button has a ""cancel"" icon with the text ""cancel"" below it.</text>
                        </binding>
                    </visual>
                    <actions>

                        <action
                            content='check'
                            imageUri='Assets/check.png'
                            activationType='foreground'
                            arguments='check'/>

                        <action
                            content='cancel'
                            imageUri='Assets/cancel.png'
                            activationType='foreground'
                            arguments='cancel'/>

                    </actions>
                </toast>";
        }

        private async void Error(string message)
        {
            await new MessageDialog(message, "ERROR").ShowAsync();
        }
    }
}
