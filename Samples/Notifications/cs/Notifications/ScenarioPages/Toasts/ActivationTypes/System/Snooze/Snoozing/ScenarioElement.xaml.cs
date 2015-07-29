using BackgroundTasks.Helpers;
using System;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.ActivationTypes.System.Snooze.Snoozing
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

            // Pop one to be snoozed for 1 min
            ToastHelper.PopCustomToast(
                $@"
                <toast scenario='reminder' launch='args'>
                    <visual>
                        <binding template='ToastGeneric'>
                            <text>System Snoozing (1 min)</text>
                            <text>Make sure ""1 minute - PICK ME"" is selected, and click ""snooze"". This was created at {DateTime.Now.ToString("t")}.</text>
                        </binding>
                    </visual>

                    <actions>

                        <input id='snoozeTime' type='selection' defaultInput='1'>
                            <selection id='1' content = '1 minute - PICK ME'/>
                            <selection id='2' content = '2 minutes'/>
                            <selection id='5' content = '5 minutes'/>
                        </input>

                        <action activationType='system' arguments='snooze' hint-inputId='snoozeTime' content='snooze' />

                        <action activationType='system' arguments='dismiss' content='dismiss' />

                    </actions>
                </toast>"
                );

            // And another to be snoozed for 2 mins
            ToastHelper.PopCustomToast(
                $@"
                <toast scenario='reminder' launch='args'>
                    <visual>
                        <binding template='ToastGeneric'>
                            <text>System Snoozing (2 min)</text>
                            <text>Make sure ""2 minutes - PICK ME"" is selected, and click ""snooze"". This was created at {DateTime.Now.ToString("t")}</text>
                        </binding>
                    </visual>

                    <actions>

                        <input id='snoozeTime' type='selection' defaultInput='2'>
                            <selection id='1' content = '1 minute'/>
                            <selection id='2' content = '2 minutes -  PICK ME'/>
                            <selection id='5' content = '5 minutes'/>
                        </input>

                        <action activationType='system' arguments='snooze' hint-inputId='snoozeTime' content='snooze' />

                        <action activationType='system' arguments='dismiss' content='dismiss' />

                    </actions>
                </toast>"
                );

            DispatcherTimer timerWaitForTwoMins = new DispatcherTimer()
            {
                Interval = TimeSpan.FromMinutes(2)
            };
            timerWaitForTwoMins.Tick += TimerWaitForTwoMins_Tick;
            timerWaitForTwoMins.Start();
        }

        private void TimerWaitForTwoMins_Tick(object sender, object e)
        {
            (sender as DispatcherTimer).Stop();

            stepsControl.IsNextEnabled = true;
        }

        private async void Error(string message)
        {
            await new MessageDialog(message, "ERROR").ShowAsync();
        }
    }
}
