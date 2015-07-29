using BackgroundTasks.Helpers;
using Windows.UI.Notifications;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.ToastNotificationManagerHistory.RemoveGroup.ByGroup
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ScenarioElement : UserControl
    {
        private static readonly string TAG_1 = "tag1";
        private static readonly string TAG_2 = "tag2";
        private static readonly string TAG_3 = "tag3";

        private static readonly string GROUP_1 = "group1";
        private static readonly string GROUP_2 = "group2";

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
            ToastHelper.PopToast("Group 1, Toast 1 - TO BE REMOVED", "This is the first toast in group one", TAG_1, GROUP_1);
            ToastHelper.PopToast("Group 1, Toast 2 - TO BE REMOVED", "This is the second toast in group one", TAG_2, GROUP_1);
            ToastHelper.PopToast("Group 1, Toast 3 - TO BE REMOVED", "This is the third toast in group one", TAG_3, GROUP_1);

            ToastHelper.PopToast("Group 2, Toast 1", "This is the first toast in group two", TAG_1, GROUP_2);
            ToastHelper.PopToast("Group 2, Toast 2", "This is the second toast in group two", TAG_2, GROUP_2);
        }

        private void ButtonRemoveGroup1_Click(object sender, RoutedEventArgs e)
        {
            ToastNotificationManager.History.RemoveGroup(GROUP_1);
        }
    }
}
