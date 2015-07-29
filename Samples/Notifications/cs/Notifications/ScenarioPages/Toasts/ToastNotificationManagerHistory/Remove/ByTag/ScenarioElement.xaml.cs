using BackgroundTasks.Helpers;
using Windows.UI.Notifications;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.ToastNotificationManagerHistory.Remove.ByTag
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

            // Pop the toasts
            ToastHelper.PopToast("Toast 1", "This is the first toast", "toast1", null);
            ToastHelper.PopToast("Toast 2 - TO BE REMOVED", "This is the second toast, should be removed when you click the button", "toast2", null);
            ToastHelper.PopToast("Toast 3", "This is the third toast", "toast3", null);
        }

        private void ButtonRemoveToast2_Click(object sender, RoutedEventArgs e)
        {
            ToastNotificationManager.History.Remove("toast2");
        }
    }
}
