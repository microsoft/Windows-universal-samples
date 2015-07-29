using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.Audio.Looping
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
            // Specify the toast payload
            popToastControl.Payload =
$@"<toast scenario='incomingCall'>
    <visual>
        <binding template='ToastGeneric'>
            <text>Looping Audio Toast</text>
            <text>This toast uses looping audio, useful for scenarios like phone calls.</text>
        </binding>
    </visual>
    <actions>
        <action arguments = 'answer'
                content = 'answer' />
        <action arguments = 'ignore'
                content = 'ignore' />
    </actions>

    <audio src='ms-winsoundevent:Notification.Looping.Call3' loop='true'/>

</toast>";
        }
    }
}
