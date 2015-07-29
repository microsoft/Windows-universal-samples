// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

using System;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplateCS;

namespace LockScreenAppsCS
{
    public sealed partial class ScenarioInput1 : Page
    {
        // A pointer back to the main page which is used to gain access to the input and output frames and their content.
        MainPage rootPage = null;

        public ScenarioInput1()
        {
            InitializeComponent();
            RequestLockScreenAccess.Click += new RoutedEventHandler(RequestLockScreenAccess_Click);
            RemoveLockScreenAccess.Click += new RoutedEventHandler(RemoveLockScreenAccess_Click);
            QueryLockScreenAccess.Click += new RoutedEventHandler(QueryLockScreenAccess_Click);
        }

        private async void RequestLockScreenAccess_Click(object sender, RoutedEventArgs e)
        {
            BackgroundAccessStatus status = BackgroundAccessStatus.Unspecified;
            try
            {
                status = await BackgroundExecutionManager.RequestAccessAsync();
            }
            catch (UnauthorizedAccessException)
            {
                // An access denied exception may be thrown if two requests are issued at the same time
                // For this specific sample, that could be if the user double clicks "Request access"
            }

            switch (status)
            {
                case BackgroundAccessStatus.AllowedWithAlwaysOnRealTimeConnectivity:
                    rootPage.NotifyUser("This app is on the lock screen and has access to Always-On Real Time Connectivity.", NotifyType.StatusMessage);
                    break;
                case BackgroundAccessStatus.AllowedMayUseActiveRealTimeConnectivity:
                    rootPage.NotifyUser("This app is on the lock screen and has access to Active Real Time Connectivity.", NotifyType.StatusMessage);
                    break;
                case BackgroundAccessStatus.Denied:
                    rootPage.NotifyUser("This app is not on the lock screen.", NotifyType.StatusMessage);
                    break;
                case BackgroundAccessStatus.Unspecified:
                    rootPage.NotifyUser("The user has not yet taken any action. This is the default setting and the app is not on the lock screen.", NotifyType.StatusMessage);
                    break;
                default:
                    break;
            }
        }

        private void RemoveLockScreenAccess_Click(object sender, RoutedEventArgs e)
        {
            BackgroundExecutionManager.RemoveAccess();
            rootPage.NotifyUser("This app has been removed from the lock screen.", NotifyType.StatusMessage);
        }

        private void QueryLockScreenAccess_Click(object sender, RoutedEventArgs e)
        {
            switch (BackgroundExecutionManager.GetAccessStatus())
            {
                case BackgroundAccessStatus.AllowedWithAlwaysOnRealTimeConnectivity:
                    rootPage.NotifyUser("This app is on the lock screen and has access to Always-On Real Time Connectivity.", NotifyType.StatusMessage);
                    break;
                case BackgroundAccessStatus.AllowedMayUseActiveRealTimeConnectivity:
                    rootPage.NotifyUser("This app is on the lock screen and has access to Active Real Time Connectivity.", NotifyType.StatusMessage);
                    break;
                case BackgroundAccessStatus.Denied:
                    rootPage.NotifyUser("This app is not on the lock screen.", NotifyType.StatusMessage);
                    break;
                case BackgroundAccessStatus.Unspecified:
                    rootPage.NotifyUser("The user has not yet taken any action. This is the default setting and the app is not on the lock screen.", NotifyType.StatusMessage);
                    break;
                default:
                    break;
            }
        }


        #region Template-Related Code - Do not remove
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Get a pointer to our main page
            rootPage = e.Parameter as MainPage;
        }

        #endregion
    }
}
