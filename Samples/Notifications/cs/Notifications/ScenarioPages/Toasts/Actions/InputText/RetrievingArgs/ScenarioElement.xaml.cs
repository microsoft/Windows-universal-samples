using System;
using Windows.ApplicationModel.Activation;
using Windows.ApplicationModel.Core;
using Windows.Foundation.Collections;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.Actions.InputText.RetrievingArgs
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ScenarioElement : UserControl
    {
        public ScenarioElement()
        {
            this.InitializeComponent();
            
            Initialize();
        }

        private void Initialize()
        {
            // Clear all existing notifications
            ToastNotificationManager.History.Clear();

            // Listen to activation event
            (Application.Current as App).Activated = new EventHandler<IActivatedEventArgs>(App_Activated);

            // Pop notifications
            popToastControlStep1.Payload =
                $@"
                <toast activationType='foreground' launch='args'>
                    <visual>
                        <binding template='ToastGeneric'>
                            <text>Foreground with App Open</text>
                            <text>Make sure ""Windows 10"" is in the text box. Press ""submit"".</text>
                        </binding>
                    </visual>
                    <actions>

                        <input
                            id = 'message'
                            type = 'text'
                            title = 'Message'
                            placeHolderContent = 'Enter ""Windows 10""'
                            defaultInput = 'Windows 10' />

                        <action activationType = 'foreground'
                                arguments = 'quickReply'
                                content = 'submit' />

                        <action activationType = 'foreground'
                                arguments = 'cancel'
                                content = 'cancel' />

                    </actions>
                </toast>";
        }

        private async void App_Activated(object sender, IActivatedEventArgs e)
        {
            await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, delegate
            {
                var toastArgs = e as ToastNotificationActivatedEventArgs;
                if (toastArgs == null)
                    return;

                string arguments = toastArgs.Argument;

                if (arguments == null || !arguments.Equals("quickReply"))
                {
                    Error($"Expected arguments to be 'quickReply' but was '{arguments}'.");
                    return;
                }

                switch (stepsControl.Step)
                {
                    case 1:
                        validateStep1(toastArgs.UserInput);
                        break;
                }
            });
        }

        private async void Error(string message)
        {
            await new MessageDialog(message, "ERROR").ShowAsync();
        }

        private async void validateStep1(ValueSet result)
        {
            if (result.Count != 1)
                await new MessageDialog("ERROR: Expected 1 user input value, but there were " + result.Count).ShowAsync();

            else if (!result.ContainsKey("message"))
                await new MessageDialog("ERROR: Expected a user input value for 'message', but there was none.").ShowAsync();

            else if (!(result["message"] as string).Equals("Windows 10"))
                await new MessageDialog("ERROR: User input value for 'message' was not 'Windows 10'").ShowAsync();

            else
            {
                stepsControl.Step = int.MaxValue;
            }
        }
    }
}
