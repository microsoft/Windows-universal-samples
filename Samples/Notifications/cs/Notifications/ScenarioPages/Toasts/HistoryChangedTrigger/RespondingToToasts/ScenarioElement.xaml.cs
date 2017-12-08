using BackgroundTasks.Helpers;
using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.ApplicationModel.Core;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.HistoryChangedTrigger.RespondingToToasts
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ScenarioElement : UserControl
    {
        private static readonly string BACKGROUND_TASK_NAME = "ToastNotificationHistoryChangedTriggerTask";

        private ObservableCollection<ToastHistoryChangedType> _changeEvents = new ObservableCollection<ToastHistoryChangedType>();

        public ScenarioElement()
        {
            this.InitializeComponent();

            ItemsControlChangeEvents.ItemsSource = _changeEvents;

            Initialize();
        }

        private int _step = 1;

        private async void Initialize()
        {
            scrollViewer.IsEnabled = false;

            // Clear all existing notifications
            ToastNotificationManager.History.Clear();

            // Register background task
            if (!await RegisterBackgroundTask())
            {
                await new MessageDialog("ERROR: Couldn't register background task.").ShowAsync();
                return;
            }

            // Pop all the notifications
            ToastHelper.PopToast("ignore me 1", "Content of ignore me 1");
            ToastHelper.PopToast("SWIPE ME", "Swipe me away");
            ToastHelper.PopToast("ignore me 2", "Content of ignore me 2");

            StackPanelStep1.Visibility = Visibility.Visible;

            scrollViewer.IsEnabled = true;
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
                TaskEntryPoint = "BackgroundTasks.ToastNotificationHistoryChangedTriggerTask"
            };

            // Set trigger for Toast History Changed
            builder.SetTrigger(new ToastNotificationHistoryChangedTrigger());

            // And register the background task
            BackgroundTaskRegistration registration = builder.Register();

            // And then listen for when the background task updates progress (it passes the toast change type)
            registration.Progress += BackgroundTask_Progress;

            return true;
        }

        private void BackgroundTask_Progress(BackgroundTaskRegistration sender, BackgroundTaskProgressEventArgs args)
        {
            ToastHistoryChangedType changedType = (ToastHistoryChangedType)args.Progress;

            CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, delegate
            {
                OnHistoryChanged(changedType);
            });
        }

        private DispatcherTimer _timerStep3;

        private async void OnHistoryChanged(ToastHistoryChangedType changedType)
        {
            _changeEvents.Add(changedType);

            switch (_step)
            {
                case 1:

                    if (changedType == ToastHistoryChangedType.Removed)
                    {
                        _step = 2;
                        StackPanelStep1.Visibility = Visibility.Collapsed;
                        StackPanelStep2.Visibility = Visibility.Visible;
                    }

                    else
                    {
                        await new MessageDialog("ERROR: Step 1 - expected Removed but was " + changedType).ShowAsync();
                    }

                    break;


                case 2:

                    if (changedType == ToastHistoryChangedType.Removed)
                    {
                        _step = 3;
                        StackPanelStep2.Visibility = Visibility.Collapsed;
                        StackPanelStep3.Visibility = Visibility.Visible;

                        _timerStep3 = new DispatcherTimer()
                        {
                            Interval = TimeSpan.FromSeconds(10)
                        };
                        _timerStep3.Tick += _timerStep3_Tick;
                        _timerStep3.Start();
                    }

                    else
                    {
                        await new MessageDialog("ERROR: Step 2 - expected Removed but was " + changedType).ShowAsync();
                    }

                    break;


                case 3:

                    if (_timerStep3 != null)
                        _timerStep3.Stop();

                    await new MessageDialog("ERROR: Step 3 - wasn't expecting any changes, but received " + changedType).ShowAsync();
                    break;


                case 4:
                    break;


                default:
                    throw new NotImplementedException("Didn't handle step " + _step);

            }
        }

        private void _timerStep3_Tick(object sender, object e)
        {
            (sender as DispatcherTimer).Stop();

            _step = 4;
            StackPanelStep3.Visibility = Visibility.Collapsed;
            StackPanelStep4.Visibility = Visibility.Visible;
        }

        private static void UnregisterBackgroundTask()
        {
            var task = BackgroundTaskRegistration.AllTasks.Values.FirstOrDefault(i => i.Name.Equals(BACKGROUND_TASK_NAME));

            if (task != null)
                task.Unregister(true);
        }
    }
}
