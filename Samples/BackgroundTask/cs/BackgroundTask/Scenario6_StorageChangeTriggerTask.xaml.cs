using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Windows.ApplicationModel.Background;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.Storage;
using Windows.UI.Core;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class StorageChangeTriggerTask : Page
    {
        private MainPage rootPage = MainPage.Current;

        public StorageChangeTriggerTask()
        {
            this.InitializeComponent();
        }
        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            foreach (var task in BackgroundTaskRegistration.AllTasks)
            {
                if (task.Value.Name == BackgroundTaskSample.StorageChangeTriggerTaskName)
                {
                    AttachProgressAndCompletedHandlers(task.Value);
                    BackgroundTaskSample.UpdateBackgroundTaskStatus(BackgroundTaskSample.StorageChangeTriggerTaskName, true);
                    break;
                }
            }

            UpdateUI();
        }

        /// <summary>
        /// Register a SampleBackgroundTask.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void RegisterBackgroundTask(object sender, RoutedEventArgs e)
        {
            StorageLibrary lib = await StorageLibrary.GetLibraryAsync(KnownLibraryId.Pictures);
            StorageLibraryContentChangedTrigger libraryTrigger = StorageLibraryContentChangedTrigger.Create(lib);

            var task = BackgroundTaskSample.RegisterBackgroundTask(BackgroundTaskSample.StorageChangeTriggerTaskEntryPoint,
                                                                   BackgroundTaskSample.StorageChangeTriggerTaskName,
#if true
                                                                   libraryTrigger,
#else
                                                                   new SystemTrigger(SystemTriggerType.TimeZoneChange, false),
#endif
                                                                   null);
            await task;
            AttachProgressAndCompletedHandlers(task.Result);
            UpdateUI();
        }

        /// <summary>
        /// Unregister a SampleBackgroundTask.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void UnregisterBackgroundTask(object sender, RoutedEventArgs e)
        {
            BackgroundTaskSample.UnregisterBackgroundTasks(BackgroundTaskSample.StorageChangeTriggerTaskName);
            UpdateUI();
        }

        /// <summary>
        /// Attach progress and completed handers to a background task.
        /// </summary>
        /// <param name="task">The task to attach progress and completed handlers to.</param>
        private void AttachProgressAndCompletedHandlers(IBackgroundTaskRegistration task)
        {
            task.Progress += new BackgroundTaskProgressEventHandler(OnProgress);
            task.Completed += new BackgroundTaskCompletedEventHandler(OnCompleted);
        }

        /// <summary>
        /// Handle background task progress.
        /// </summary>
        /// <param name="task">The task that is reporting progress.</param>
        /// <param name="e">Arguments of the progress report.</param>
        private void OnProgress(IBackgroundTaskRegistration task, BackgroundTaskProgressEventArgs args)
        {
            var progress = "Progress: " + args.Progress + "%";
            BackgroundTaskSample.StorageChangeTriggerTaskProgress = progress;
            UpdateUI();
        }

        /// <summary>
        /// Handle background task completion.
        /// </summary>
        /// <param name="task">The task that is reporting completion.</param>
        /// <param name="e">Arguments of the completion report.</param>
        private void OnCompleted(IBackgroundTaskRegistration task, BackgroundTaskCompletedEventArgs args)
        {
            UpdateUI();
        }

        /// <summary>
        /// Update the scenario UI.
        /// </summary>
        private async void UpdateUI()
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
            () =>
            {
                RegisterButton.IsEnabled = !BackgroundTaskSample.StorageChangeTriggerTaskRegistered;
                UnregisterButton.IsEnabled = BackgroundTaskSample.StorageChangeTriggerTaskRegistered;
                Progress.Text = BackgroundTaskSample.StorageChangeTriggerTaskProgress;
                Status.Text = BackgroundTaskSample.GetBackgroundTaskStatus(BackgroundTaskSample.StorageChangeTriggerTaskName);
            });
        }
    }
}
