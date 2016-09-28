using BackgroundTasks;
using BackgroundTasks.Helpers;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.HistoryChangedTrigger.BadgeControl
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ScenarioElement : UserControl
    {
        private static readonly string BACKGROUND_TASK_NAME = "ToastNotificationHistoryChangedTriggerTask";

        public ScenarioElement()
        {
            this.InitializeComponent();

            Initialize();
        }

        private async void Initialize()
        {
            base.IsEnabled = false;

            // Clear all existing notifications
            ToastNotificationManager.History.Clear();

            // Register background task
            if (!await RegisterBackgroundTask())
            {
                await new MessageDialog("ERROR: Couldn't register background task.").ShowAsync();
                return;
            }


            base.IsEnabled = true;
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
                Name = BACKGROUND_TASK_NAME,
                TaskEntryPoint = "BackgroundTasks.BadgeNotificationHistoryChangedTriggerTask"
            };

            // Set trigger for Toast History Changed
            builder.SetTrigger(new ToastNotificationHistoryChangedTrigger());

            // And register the background task
            BackgroundTaskRegistration registration = builder.Register();

            return true;
        }

        private static void UnregisterBackgroundTask()
        {
            var task = BackgroundTaskRegistration.AllTasks.Values.FirstOrDefault(i => i.Name.Equals(BACKGROUND_TASK_NAME));

            if (task != null)
                task.Unregister(true);
        }

        private string _tileId;
        private async void ButtonPinSecondaryTile_Click(object sender, RoutedEventArgs e)
        {
            _tileId = "ScenarioBadgeControl";

            SecondaryTile tile = new SecondaryTile(_tileId, "Name 1", "args", new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Medium.png"), TileSize.Default);
            tile.VisualElements.Square71x71Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Small.png");
            tile.VisualElements.Wide310x150Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Wide.png");
            tile.VisualElements.Square310x310Logo = new Uri("ms-appx:///Assets/DefaultSecondaryTileAssests/Large.png");
            tile.VisualElements.ShowNameOnSquare150x150Logo = true;
            tile.VisualElements.ShowNameOnSquare310x310Logo = true;
            tile.VisualElements.ShowNameOnWide310x150Logo = true;
            await tile.RequestCreateAsync();

            //Prepare for next Step:

            // Pop all the notifications
            ToastHelper.PopToast("Toast 1", "Content of Toast 1");
            ToastHelper.PopToast("Toast 2", "Content of Toast 2");
            ToastHelper.PopToast("Toast 3", "Content of Toast 3");

            //Update badge
            IReadOnlyList<ToastNotification> TNList = ToastNotificationManager.History.GetHistory();

            BadgeNumericNotificationContent badgeContent = new BadgeNumericNotificationContent((uint)TNList.Count);

            // Send the notification to the application’s tile.
            BadgeUpdateManager.CreateBadgeUpdaterForSecondaryTile(_tileId).Update(badgeContent.CreateNotification());

            // Move the UI to the next step
            stepsControl.Step++;
            stepsControl.NextButtonVisibility = Visibility.Collapsed;
        }
    }
}
