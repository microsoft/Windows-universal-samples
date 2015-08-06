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
using System.Collections.ObjectModel;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.System;
using Windows.UI.Core;
using System.Threading.Tasks;

namespace SDKTemplate
{
    public sealed partial class Scenario2_WatchUsers : Page
    {
        MainPage rootPage = MainPage.Current;
        public ObservableCollection<UserViewModel> Users = new ObservableCollection<UserViewModel>();
        int userNumber = 1;
        UserWatcher userWatcher = null;

        public Scenario2_WatchUsers()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            StopWatching();
        }

        private void StartWatching()
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            userNumber = 1;
            Users.Clear();
            userWatcher = User.CreateWatcher();
            userWatcher.Added += OnUserAdded;
            userWatcher.Updated += OnUserUpdated;
            userWatcher.Removed += OnUserRemoved;
            userWatcher.EnumerationCompleted += OnEnumerationCompleted;
            userWatcher.Stopped += OnWatcherStopped;
            userWatcher.Start();
            StartButton.IsEnabled = false;
            StopButton.IsEnabled = true;
        }


        private void StopWatching()
        {
            if (userWatcher != null)
            {
                // Unregister all event handlers in case events are in flight.
                userWatcher.Added -= OnUserAdded;
                userWatcher.Updated -= OnUserUpdated;
                userWatcher.Removed -= OnUserRemoved;
                userWatcher.EnumerationCompleted -= OnEnumerationCompleted;
                userWatcher.Stopped -= OnWatcherStopped;
                userWatcher.Stop();
                userWatcher = null;
                Users.Clear();
                StartButton.IsEnabled = true;
                StopButton.IsEnabled = false;
            }
        }

        private UserViewModel FindModelByUserId(string userId)
        {
            foreach (UserViewModel model in Users)
            {
                if (model.UserId == userId)
                {
                    return model;
                }
            }
            return null;
        }

        private async Task<String> GetDisplayNameOrGenericNameAsync(User user)
        {
            // Try to get the display name.
            string displayName = (string)await user.GetPropertyAsync(KnownUserProperties.DisplayName);

            // Choose a generic name if we do not have access to the actual name.
            if (String.IsNullOrEmpty(displayName))
            {
                displayName = "User #" + userNumber.ToString();
                userNumber++;
            }
            return displayName;
        }

        private async void OnUserAdded(UserWatcher sender, UserChangedEventArgs e)
        {
            User user = e.User;

            // UI work must happen on the UI thread.
            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Low, async () =>
            {
                // Create the user with "..." as the temporary display name.
                // Add it right away, because it might get removed while the
                // "await GetDisplayNameOrGenericNameAsync()" is running.
                var model = new UserViewModel(user.NonRoamableId, "\u2026");
                Users.Add(model);

                // Try to get the display name.
                model.DisplayName = await GetDisplayNameOrGenericNameAsync(user);
            });
        }

        private async void OnUserUpdated(UserWatcher sender, UserChangedEventArgs e)
        {
            User user = e.User;

            // UI work must happen on the UI thread.
            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Low, async () =>
            {
                // Look for the user in our collection and update the display name.
                UserViewModel model = FindModelByUserId(user.NonRoamableId);
                if (model != null)
                {
                    model.DisplayName = await GetDisplayNameOrGenericNameAsync(user);
                }
            });
        }

        private async void OnUserRemoved(UserWatcher sender, UserChangedEventArgs e)
        {
            User user = e.User;

            // UI work must happen on the UI thread.
            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
            {
                // Look for the user in our collection and remove it.
                UserViewModel model = FindModelByUserId(user.NonRoamableId);
                if (model != null)
                {
                    Users.Remove(model);
                }
            });
        }

        private async void OnEnumerationCompleted(UserWatcher sender, Object e)
        {
            // UI work must happen on the UI thread.
            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
            {
                rootPage.NotifyUser("Enumeration complete. Watching for changes...", NotifyType.StatusMessage);
            });
        }

        private async void OnWatcherStopped(UserWatcher sender, Object e)
        {
            // UI work must happen on the UI thread.
            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
            {
                StopWatching();
            });
        }
    }
}
