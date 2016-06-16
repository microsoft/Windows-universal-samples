using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel.ExtendedExecution;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class MultipleTasks : Page
    {
        private MainPage rootPage = MainPage.Current;

        private ExtendedExecutionSession session = null;
        private CancellationTokenSource cancellationTokenSource;
        private int taskCount = 0;

        public MultipleTasks()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            // End the Extended Execution Session.
            // Only one extended execution session can be held by an application at one time.
            ClearExtendedExecution();
        }

        void ClearExtendedExecution()
        {
            // Cancel any outstanding tasks.
            if (cancellationTokenSource != null)
            {
                // Save a copy of cancellationTokenSource because the call
                // to cancellationTokenSource.Cancel() will cause other code
                // to run, which might in turn call ClearExtendedExecution.
                var localCancellationTokenSource = cancellationTokenSource;
                cancellationTokenSource = null;

                localCancellationTokenSource.Cancel();
                localCancellationTokenSource.Dispose();
            }

            // Dispose any outstanding session.
            if (session != null)
            {
                session.Revoked -= SessionRevoked;
                session.Dispose();
                session = null;
            }
        }

        private void UpdateUI()
        {
            if (session == null)
            {
                Status.Text = "Not requested";
                RequestButton.IsEnabled = true;
                CloseButton.IsEnabled = false;

            }
            else
            {
                Status.Text = "Requested";
                RequestButton.IsEnabled = false;
                CloseButton.IsEnabled = true;
            }
        }

        private void OnTaskCompleted()
        {
            taskCount--;
            if (taskCount == 0 && session != null)
            {
                EndExtendedExecution();
                MainPage.DisplayToast("All Tasks Completed, ending Extended Execution.");
            }
        }

        private Deferral GetExecutionDeferral()
        {
            if (session == null)
            {
                throw new InvalidOperationException("No extended execution session is active");
            }

            taskCount++;
            return new Deferral(OnTaskCompleted);
        }

        private async void BeginExtendedExecution()
        {
            // The previous Extended Execution must be closed before a new one can be requested.
            // This code is redundant here because the sample doesn't allow a new extended
            // execution to begin until the previous one ends, but we leave it here for illustration.
            ClearExtendedExecution();

            var newSession = new ExtendedExecutionSession();
            newSession.Reason = ExtendedExecutionReason.Unspecified;
            newSession.Description = "Running multiple tasks";
            newSession.Revoked += SessionRevoked;
            ExtendedExecutionResult result = await newSession.RequestExtensionAsync();

            switch (result)
            {
                case ExtendedExecutionResult.Allowed:
                    rootPage.NotifyUser("Extended execution allowed.", NotifyType.StatusMessage);
                    session = newSession;
                    break;

                default:
                case ExtendedExecutionResult.Denied:
                    rootPage.NotifyUser("Extended execution denied.", NotifyType.ErrorMessage);
                    newSession.Dispose();
                    break;
            }
            UpdateUI();

            if (session != null)
            {
                cancellationTokenSource = new CancellationTokenSource();

                // Start up a few tasks that all share this session.
                using (var deferral = GetExecutionDeferral())
                {
                    var random = new Random();
                    for (int i = 0; i < 3; i++)
                    {
                        RaiseToastAfterDelay(i, random.Next(5, 10));
                    }
                }
            }
        }

        private async void RaiseToastAfterDelay(int id, int seconds)
        {
            using (var deferral = GetExecutionDeferral())
            {
                try
                {
                    await Task.Delay(seconds * 1000, cancellationTokenSource.Token);
                    MainPage.DisplayToast($"Task {id} completed after {seconds} seconds");
                }
                catch (TaskCanceledException)
                {
                    MainPage.DisplayToast($"Task {id} canceled");
                }
            }
        }

        private void EndExtendedExecution()
        {
            ClearExtendedExecution();
            UpdateUI();
        }

        private async void SessionRevoked(object sender, ExtendedExecutionRevokedEventArgs args)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                switch (args.Reason)
                {
                    case ExtendedExecutionRevokedReason.Resumed:
                        rootPage.NotifyUser("Extended execution revoked due to returning to foreground.", NotifyType.StatusMessage);
                        break;

                    case ExtendedExecutionRevokedReason.SystemPolicy:
                        rootPage.NotifyUser("Extended execution revoked due to system policy.", NotifyType.StatusMessage);
                        break;
                }

                ClearExtendedExecution();
            });
        }
    }
}
