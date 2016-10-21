using BackgroundTasks.Helpers;
using System;
using System.Linq;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.ApplicationModel.Core;
using Windows.Storage;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.ActivationTypes.BackgroundWithAppOpen
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ScenarioElement : UserControl
    {
        private static readonly string BACKGROUND_ENTRY_POINT = typeof(BackgroundTasks.ToastNotificationQuickReplyTask).FullName;

        public ScenarioElement()
        {
            this.InitializeComponent();

            Initialize();
        }

        private async void Initialize()
        {
            // Clear all existing notifications
            ToastNotificationManager.History.Clear();

            // Register background task
            if (!await RegisterBackgroundTask())
            {
                await new MessageDialog("ERROR: Couldn't register background task.").ShowAsync();
                return;
            }

            // Pop notifications
            popToastControl.Payload =
                $@"
                <toast activationType='background' launch='args'>
                    <visual>
                        <binding template='ToastGeneric'>
                            <text>Background with App Open</text>
                            <text>Make sure ""Windows 10"" is in the first text box. Press ""submit"".</text>
                        </binding>
                    </visual>
                    <actions>

                        <input
                            id = 'message'
                            type = 'text'
                            title = 'Message'
                            placeHolderContent = 'Enter ""Windows 10""'
                            defaultInput = 'Windows 10' />

                        <action activationType = 'background'
                                arguments = 'quickReply'
                                content = 'submit' />

                        <action activationType = 'background'
                                arguments = 'cancel'
                                content = 'cancel' />

                    </actions>
                </toast>";
        }

        private async Task<bool> RegisterBackgroundTask()
        {
            // Unregister any previous exising background task
            UnregisterBackgroundTask();

            // Request access
            BackgroundAccessStatus status = await BackgroundExecutionManager.RequestAccessAsync();

            // If denied
            if (status != BackgroundAccessStatus.AlwaysAllowed && status != BackgroundAccessStatus.AllowedSubjectToSystemPolicy)
                return false;

            // Construct the background task
            BackgroundTaskBuilder builder = new BackgroundTaskBuilder()
            {
                Name = BACKGROUND_ENTRY_POINT,
                TaskEntryPoint = BACKGROUND_ENTRY_POINT
            };

            // Set trigger for Toast History Changed
            builder.SetTrigger(new ToastNotificationActionTrigger());

            // And register the background task
            BackgroundTaskRegistration registration = builder.Register();

            // And then listen for when the background task updates progress (it passes the toast change type)
            registration.Completed += BackgroundTask_Completed;

            return true;
        }

        private async void BackgroundTask_Completed(BackgroundTaskRegistration sender, BackgroundTaskCompletedEventArgs args)
        {
            await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, delegate
            {
                string error = BackgroundTaskStorage.GetError();

                if (error != null)
                {
                    new MessageDialog(error, "Error").ShowAsync();
                    return;
                }

                ApplicationDataCompositeValue result = (ApplicationDataCompositeValue)BackgroundTaskStorage.GetAnswer();

                OnResultReceived(result);

            });
        }

        private void OnResultReceived(ApplicationDataCompositeValue result)
        {
            switch (stepsControl.Step)
            {
                case 1:
                    validateStep1(result);
                    break;
            }
        }

        private async void validateStep1(ApplicationDataCompositeValue result)
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
                UnregisterBackgroundTask();
            }
        }

        private static void UnregisterBackgroundTask()
        {
            var task = BackgroundTaskRegistration.AllTasks.Values.FirstOrDefault(i => i.Name.Equals(BACKGROUND_ENTRY_POINT));

            if (task != null)
                task.Unregister(true);
        }
    }
}
