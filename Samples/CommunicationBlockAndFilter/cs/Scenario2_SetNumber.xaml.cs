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
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;
using Windows.ApplicationModel.Calls;
using Windows.ApplicationModel.CommunicationBlocking;
using Windows.Foundation;
using Windows.Storage;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2_SetNumbers : Page
    {
        private MainPage rootPage;
        public ObservableCollection<string> BlockedNumbers;

        public Scenario2_SetNumbers()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            // Load the blocked numbers from our app's private storage.
            // Note that ApplicationData.Current.LocalSettings is not designed
            // to store large quantities of data, so a real (non-sample) app
            // should store the information in a file or in a Web service.

            // If there are no saved blocked numbers, then default to an empty list.
            var blockedNumbers = ApplicationData.Current.LocalSettings.Values["blockedNumbers"] as string[];
            BlockedNumbers = new ObservableCollection<string>(blockedNumbers ?? new string[] { });

            UpdateWarning();
        }

        private void UpdateWarning()
        {
            if (CommunicationBlockingAppManager.IsCurrentAppActiveBlockingApp)
            {
                ActiveAppWarning.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }
            else
            {
                ActiveAppWarning.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
        }

        private async Task UpdateBlockListAsync()
        {
            await PhoneCallBlocking.SetCallBlockingListAsync(BlockedNumbers);

            // Save the new blocked numbers list in our private storage.
            var settings = ApplicationData.Current.LocalSettings;
            if (BlockedNumbers.Count > 0)
            {
                settings.Values["blockedNumbers"] = BlockedNumbers.ToArray<string>();
            }
            else
            {
                settings.Values["blockedNumbers"] = null;
            }
            
        }

        private async void AddBlockedNumber()
        {
            UpdateWarning();

            // The system can block at most 10,000 numbers.
            if (BlockedNumbers.Count >= 10000)
            {
                rootPage.NotifyUser("Too many blocked numbers.", NotifyType.ErrorMessage);
                return;
            }

            BlockedNumbers.Add(BlockNumberText.Text);

            await UpdateBlockListAsync();

            rootPage.NotifyUser(BlockNumberText.Text + " is blocked.", NotifyType.StatusMessage);
        }

        private async void RemoveBlockedNumber()
        {
            UpdateWarning();

            int index = BlockedNumbersList.SelectedIndex;
            if (index < 0)
            {
                rootPage.NotifyUser("No number selected", NotifyType.ErrorMessage);
                return;
            }

            string unblockedNumber = BlockedNumbers[index];
            BlockedNumbers.RemoveAt(index);

            await UpdateBlockListAsync();

            rootPage.NotifyUser(unblockedNumber + " removed from block list.", NotifyType.StatusMessage);
        }
    }
}
