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
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.Data.Xml.Dom;
using Windows.Networking.NetworkOperators;
using Windows.UI.Notifications;

// The namespace for the background tasks.
namespace HotspotAuthenticationTask
{
    // A background task always implements the IBackgroundTask interface.
    public sealed class AuthenticationTask : IBackgroundTask
    {
        private const string _foregroundAppId = "HotspotAuthenticationApp.App";
        private volatile bool _cancelRequested = false;

        private HotspotAuthenticationEventDetails _details;
        private HotspotAuthenticationContext _context;

        // The Run method is the entry point of a background task.
        public async void Run(IBackgroundTaskInstance taskInstance)
        {
            Debug.WriteLine("Background " + taskInstance.Task.Name + " starting...");

            // Convert the trigger details to a HotspotAuthenticationEventDetails.
            _details = taskInstance.TriggerDetails as HotspotAuthenticationEventDetails;

            // Associate a cancelation handler with the background task for handlers
            // that may take a considerable time to complete.
            taskInstance.Canceled += OnCanceled;

            // Take a deferral so that we can call asynchronous APIs.
            BackgroundTaskDeferral deferral = taskInstance.GetDeferral();

            // The real work happens in RunAsync.
            await RunAsync();

            Debug.WriteLine("Background " + taskInstance.Task.Name + " completed");

            deferral.Complete();
        }

        private async Task RunAsync()
        {
            // Do the background task activity. First, get the authentication context.
            Debug.WriteLine("Getting event details");

            if (!HotspotAuthenticationContext.TryGetAuthenticationContext(_details.EventToken, out _context))
            {
                // The event is not targetting this application. There is no further processing to do.
                Debug.WriteLine("Failed to get event context");
                return;
            }

            byte[] ssid = _context.WirelessNetworkId;
            Debug.WriteLine("SSID: " + System.Text.UTF8Encoding.UTF8.GetString(ssid, 0, ssid.Length));

            if (ConfigStore.UseNativeWISPr)
            {
                // Following code can be used if using native WISPr implementation. Please note that 
                // following HotspotAuthenticationContext properties only work on windows and not on windows phone. 
                // On Windows Phone they return un-useful strings
                // Developers are expected to implement their own WISPr implementation on Phone

                Debug.WriteLine("AuthenticationUrl: " + _context.AuthenticationUrl.OriginalString);
                Debug.WriteLine("RedirectMessageUrl: " + _context.RedirectMessageUrl.OriginalString);
                Debug.WriteLine("RedirectMessageXml: " + _context.RedirectMessageXml.GetXml());

                // In this sample, the AuthenticationUrl is always checked in the background task handler
                // to avoid launching the foreground app in case the authentication host is not trusted.
                if (ConfigStore.AuthenticationHost != _context.AuthenticationUrl.Host)
                {
                    // Hotspot is not using the trusted authentication server.
                    // Abort authentication and disconnect.
                    Debug.WriteLine("Authentication server is untrusted");
                    _context.AbortAuthentication(ConfigStore.MarkAsManualConnect);
                    return;
                }
            }

            // Run the appropriate scenario selected by the foreground app.
            if (ConfigStore.AuthenticateThroughBackgroundTask)
            {
                await AuthenticateInBackgroundAsync();
            }
            else
            {
                ContinueAuthenticationInForeground();
            }
            return;
        }

        private async Task AuthenticateInBackgroundAsync()
        {
            // In case this handler performs more complex tasks, it may get canceled at runtime.
            // Check if task was canceled by now.
            if (_cancelRequested)
            {
                // In case the task handler takes too long to generate credentials and gets canceled,
                // the handler should terminate the authentication by aborting it
                Debug.WriteLine("Aborting authentication");
                _context.AbortAuthentication(ConfigStore.MarkAsManualConnect);
                return;
            }

            if (ConfigStore.UseNativeWISPr)
            {
                // The most common way of handling an authentication attempt is by providing WISPr credentials
                // through the IssueCredentialsAsync API.
                // If the task doesn't take any actions for authentication failures, it can use the 
                // IssueCredentials API to just provide credenstials.
                // Alternatively, an application could run its own business logic to authentication with the
                // hotspot. In this case it should call the SkipAuthentication API. Note that it should call
                // SkipAuthentication after it has authenticated to allow Windows to refresh the network connectivity
                // state instantly.
                Debug.WriteLine("Issuing credentials");
                HotspotCredentialsAuthenticationResult result = await _context.IssueCredentialsAsync(
                    ConfigStore.UserName, ConfigStore.Password, ConfigStore.ExtraParameters, ConfigStore.MarkAsManualConnect);
                if (result.ResponseCode == HotspotAuthenticationResponseCode.LoginSucceeded)
                {
                    Debug.WriteLine("Issuing credentials succeeded");
                    Uri logoffUrl = result.LogoffUrl;
                    if (logoffUrl != null)
                    {
                        Debug.WriteLine("The logoff URL is: " + logoffUrl.OriginalString);
                    }
                }
                else
                {
                    Debug.WriteLine("Issuing credentials failed");
                }
            }
            else
            {
                //TODO: Please perform any authentication that is required by your particular scenario.
                // Check _cancelRequested periodically in case the task is canceled.

                // Finally call SkipAuthentication to indicate that we are not doing native WISPr authentication
                // This call also serves the purpose of indicating a successful authentication.
                _context.SkipAuthentication();
            }
        }

        private void ContinueAuthenticationInForeground()
        {
            Debug.WriteLine("Triggering foreground application");
            // Pass event token to application
            ConfigStore.AuthenticationToken = _details.EventToken;

            // Trigger notification
            // Since TriggerAttentionRequired function throws NotImplementedException on phone we will be using
            // regular Toast Notification to notify user about the authentication, Tapping on the notification will
            // launch the application where user can complete the authentication
            if (ConfigStore.UseNativeWISPr)
            {
                // The second parameter is used as the activation command line.
                _context.TriggerAttentionRequired(_foregroundAppId, "");
            }
            else
            {
                var toastXml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText01);
                toastXml.GetElementsByTagName("text")[0].AppendChild(toastXml.CreateTextNode("Auth by foreground"));
                IXmlNode toastNode = toastXml.SelectSingleNode("/toast");
                ((XmlElement)toastNode).SetAttribute("launch", "AuthByForeground");

                var toast = new ToastNotification(toastXml);
                toast.Tag = "AuthByForeground";
                toast.Group = "HotspotAuthAPI";

                var notification = ToastNotificationManager.CreateToastNotifier();
                notification.Show(toast);
            }
        }

        // Handles background task cancellation.
        private void OnCanceled(IBackgroundTaskInstance sender, BackgroundTaskCancellationReason reason)
        {
            // Indicate that the background task is canceled.
            _cancelRequested = true;

            Debug.WriteLine("Background " + sender.Task.Name + " cancel requested...");
        }
    }
}
