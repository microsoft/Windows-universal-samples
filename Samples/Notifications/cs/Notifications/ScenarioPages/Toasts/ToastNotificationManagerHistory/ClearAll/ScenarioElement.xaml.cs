using BackgroundTasks.Helpers;
using Windows.UI.Notifications;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.ToastNotificationManagerHistory.ClearAll
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

        private void ButtonPopToasts_Click(object sender, RoutedEventArgs e)
        {
            ButtonPopToasts.IsEnabled = false;

            ToastHelper.PopToast("First toast", "This is the first toast");
            ToastHelper.PopToast("Second toast", "This is the second toast");
            ToastHelper.PopToast("Third toast", "This is the third toast");

            ButtonPopToasts.IsEnabled = true;
        }

        private void ButtonClearAll_Click(object sender, RoutedEventArgs e)
        {
            ButtonClearAll.IsEnabled = false;

            ToastNotificationManager.History.Clear();

            ButtonClearAll.IsEnabled = true;
        }
    }
}
