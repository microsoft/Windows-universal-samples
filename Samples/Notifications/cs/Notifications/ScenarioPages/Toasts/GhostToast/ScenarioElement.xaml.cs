using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.GhostToast
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

        private void ButtonTriggerGhostToast_Click(object sender, RoutedEventArgs e)
        {
            XmlDocument toastXml = new XmlDocument();
            toastXml.LoadXml($@"
                <toast>
                    <visual>
                        <binding template='ToastGeneric'>
                            <text>Ghost Toast</text>
                            <text>This toast silently appears into your Action Center.</text>
                        </binding>
                    </visual>
                </toast>");

            ToastNotification toast = new ToastNotification(toastXml)
            {
                SuppressPopup = true
            };
            ToastNotificationManager.CreateToastNotifier().Show(toast);
        }
    }
}
