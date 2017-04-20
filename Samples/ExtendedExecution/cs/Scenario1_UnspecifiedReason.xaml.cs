
//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.ApplicationModel.ExtendedExecution;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Page containing the Extended Execution Unspecified Reason scenario functions.
    /// </summary>
    public sealed partial class UnspecifiedReason : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        private ExtendedExecutionSession session = null;
        private Timer periodicTimer = null;

        public UnspecifiedReason()
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
            if (session != null)
            {
                session.Revoked -= SessionRevoked;
                session.Dispose();
                session = null;
            }

            if (periodicTimer != null)
            {
                periodicTimer.Dispose();
                periodicTimer = null;
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

        private async void BeginExtendedExecution()
        {
            // The previous Extended Execution must be closed before a new one can be requested.
            // This code is redundant here because the sample doesn't allow a new extended
            // execution to begin until the previous one ends, but we leave it here for illustration.
            ClearExtendedExecution();

            var newSession = new ExtendedExecutionSession();
            newSession.Reason = ExtendedExecutionReason.Unspecified;
            newSession.Description = "Raising periodic toasts";
            newSession.Revoked += SessionRevoked;
            ExtendedExecutionResult result = await newSession.RequestExtensionAsync();

            switch (result)
            {
                case ExtendedExecutionResult.Allowed:
                    rootPage.NotifyUser("Extended execution allowed.", NotifyType.StatusMessage);
                    session = newSession;
                    periodicTimer = new Timer(OnTimer, DateTime.Now, TimeSpan.FromSeconds(2), TimeSpan.FromSeconds(10));
                    break;

                default:
                case ExtendedExecutionResult.Denied:
                    rootPage.NotifyUser("Extended execution denied.", NotifyType.ErrorMessage);
                    newSession.Dispose();
                    break;
            }
            UpdateUI();
        }

        private void OnTimer(object state)
        {
            var startTime = (DateTime)state;
            var runningTime = Math.Round((DateTime.Now - startTime).TotalSeconds, 0);
            MainPage.DisplayToast($"Extended execution has been active for {runningTime} seconds");
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

                EndExtendedExecution();
            });
        }
    }
}
