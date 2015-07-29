using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.Audio.Default
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
$@"<toast>
    <visual>
        <binding template='ToastGeneric'>
            <text>Default Audio Toast</text>
            <text>This toast's audio is not specified, and thus uses the system default.</text>
        </binding>
    </visual>

</toast>";
        }
    }
}
