using BackgroundMediaPlayback.DataModels;
using BackgroundMediaPlayback.Services;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.Media.Playback;
using Windows.System;
using Windows.UI.Notifications;
using Windows.UI.Xaml;

namespace SDKTemplate
{
    sealed partial class App : Application
    {
        /// <summary>
        /// Flag to indicate whether the app is currently in
        /// foreground or background mode.
        /// </summary>
        /// <remarks>
        /// This is used later to determine when to load / unload UI.
        /// </remarks>
        bool isInBackgroundMode;

        /// <summary>
        /// Called from App.xaml.cs when the application is constructed.
        /// </summary>
        partial void Construct()
        {
            // When an application is backgrounded it is expected to reach
            // a lower memory target to maintain priority to keep running.
            // Subscribe to the event that informs the app of this change.
            MemoryManager.AppMemoryUsageIncreased += MemoryManager_AppMemoryUsageIncreased;

            // Subscribe to key lifecyle events to know when the app
            // transitions to and from foreground and background.
            // Leaving the background is an important transition
            // because we may need to restore UI.
            EnteredBackground += App_EnteredBackground;
            LeavingBackground += App_LeavingBackground;

            // Subscribe to regular lifecycle events to display a toast notification
            Suspending += App_Suspending;
            Resuming += App_Resuming;
        }

        /// <summary>
        /// Invoked when application execution is being suspended.  Application state is saved
        /// without knowing whether the application will be terminated or resumed with the contents
        /// of memory still intact.
        /// </summary>
        /// <param name="sender">The source of the suspend request.</param>
        /// <param name="e">Details about the suspend request.</param>
        private void App_Suspending(object sender, SuspendingEventArgs e)
        {
            var deferral = e.SuspendingOperation.GetDeferral();
            // Optional: Save application state and stop any background activity
            ShowToast("Suspending");
            deferral.Complete();
        }

        /// <summary>
        /// Invoked when application execution is resumed.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void App_Resuming(object sender, object e)
        {
            ShowToast("Resuming");
        }

        /// <summary>
        /// Handle system notifications that the app has increased its
        /// memory usage level compared to its current target.
        /// </summary>
        /// <remarks>
        /// The app may have increased its usage or the app may have moved
        /// to the background and the system lowered the target the app
        /// is expected to reach. Either way, if the application wants
        /// to maintain its priority to avoid being suspended before
        /// other apps, it needs to reduce its memory usage.
        /// </remarks>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MemoryManager_AppMemoryUsageIncreased(object sender, object e)
        {
            // Optionally log a debug message describing the increase
            ShowToast("Memory usage increased");

            // Unload view content if needed. 
            //
            // Memory usage may have been fine when initially entering the background but
            // a short time later memory targets could change or the app might
            // just be using more memory and needs to trim back.
            UnloadViewContentIfNeeded();
        }

        /// <summary>
        /// The application entered the background.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void App_EnteredBackground(object sender, EnteredBackgroundEventArgs e)
        {
            // Place the application into "background mode" and note the
            // transition with a flag.
            ShowToast("Entered background");
            isInBackgroundMode = true;

            // Unload view content if needed.
            //
            // If the AppMemoryUsageLevel changed and triggered AppMemoryUsageIncreased
            // before the app entered background mode then the view is still loaded
            // and we need to unload it now.
            UnloadViewContentIfNeeded();
        }

        /// <summary>
        /// The application is leaving the background.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void App_LeavingBackground(object sender, LeavingBackgroundEventArgs e)
        {
            // Mark the transition out of background mode.
            ShowToast("Leaving background");
            isInBackgroundMode = false;

            // Reastore view content if it was previously unloaded.
            if (Window.Current.Content == null)
            {
                ShowToast("Loading view");
                CreateRootFrame(ApplicationExecutionState.Running, string.Empty);
            } 
        }

        /// <summary>
        /// Unload view content if needed.
        /// </summary>
        /// <remarks>
        /// When the app enters the background or receives a memory usage increased
        /// event it can optionally unload its view content in order to reduce
        /// memory usage and the chance of being suspended.
        /// 
        /// This must be called from both event handlers because an application may already
        /// be in a high memory usage state when entering the background or it
        /// may be in a low memory usage state with no need to unload resources yet
        /// and only enter a higher state later.
        /// </remarks>
        public void UnloadViewContentIfNeeded()
        {
            // Obtain the current usage level
            var level = MemoryManager.AppMemoryUsageLevel;

            // Check the usage level to determine what steps to take to reduce
            // memory usage. The approach taken here is if the application is already
            // over the limit, or if it's at a high usage level, go ahead and unload
            // application views to recover memory. This is conceptually similar to
            // what earlier dual process models did except here the process continues to
            // run just without view content.
            if (level == AppMemoryUsageLevel.OverLimit || level == AppMemoryUsageLevel.High)
            {
                // Unload view for extra memory if the application is currently
                // in background mode and has a view with content.
                if (isInBackgroundMode && Window.Current.Content != null)
                {
                    ShowToast("Unloading view");

                    // Clear the view content and trigger the GC to recover resources.
                    Window.Current.Content = null;
                    GC.Collect();
                }
            }
        }

        /// <summary>
        /// Gets a string describing current memory usage
        /// </summary>
        /// <returns>String describing current memory usage</returns>
        private string GetMemoryUsageText()
        {
            return string.Format("[Memory: Level={0}, Usage={1}K, Target={2}K]", 
                MemoryManager.AppMemoryUsageLevel, MemoryManager.AppMemoryUsage / 1024, MemoryManager.AppMemoryUsageLimit / 1024);
        }

        /// <summary>
        /// Sends a toast notification
        /// </summary>
        /// <param name="msg">Message to send</param>
        /// <param name="subMsg">Sub message</param>
        public void ShowToast(string msg, string subMsg = null)
        {
            if (subMsg == null)
                subMsg = GetMemoryUsageText();

            Debug.WriteLine(msg + "\n" + subMsg);

            if (!SettingsService.Instance.ToastOnAppEvents)
                return;

            var toastXml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText02);

            var toastTextElements = toastXml.GetElementsByTagName("text");
            toastTextElements[0].AppendChild(toastXml.CreateTextNode(msg));
            toastTextElements[1].AppendChild(toastXml.CreateTextNode(subMsg));

            var toast = new ToastNotification(toastXml);
            ToastNotificationManager.CreateToastNotifier().Show(toast);
        }
    }
}
