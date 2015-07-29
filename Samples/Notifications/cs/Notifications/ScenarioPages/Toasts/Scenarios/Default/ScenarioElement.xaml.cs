using Windows.UI.Notifications;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.Scenarios.Default
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

            // Pop incoming call notification
            popToastControl.Payload =
                $@"
                <toast launch='args'>
                    <visual>
                        <binding template='ToastGeneric'>
                            <text>Default</text>
                            <text>Second Line of Text</text>
                        </binding>
                    </visual>
                    <actions>

                        <action arguments = 'ok'
                                content = 'ok' />

                        <action arguments = 'cancel'
                                content = 'cancel' />

                    </actions>
                </toast>";
        }
    }
}
