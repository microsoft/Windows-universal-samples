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
using Windows.Security.DataProtection;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.System.Update;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Files : Page
    {
        private MainPage rootPage = MainPage.Current;
        UserDataProtectionManager userDataProtectionManager = null;
        IStorageItem selectedItem = null;

        public Scenario1_Files()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            userDataProtectionManager = UserDataProtectionManager.TryGetDefault();
            if (userDataProtectionManager != null)
            {
                AvailablePanel.Visibility = Visibility.Visible;
                UnavailablePanel.Visibility = Visibility.Collapsed;
            }
            else
            {
                AvailablePanel.Visibility = Visibility.Collapsed;
                UnavailablePanel.Visibility = Visibility.Visible;
            }
        }

        private async void ChooseFile_Click(object sender, RoutedEventArgs e)
        {
            var picker = new FileOpenPicker();
            picker.FileTypeFilter.Add("*");
            UpdateItem(await picker.PickSingleFileAsync());
        }

        private async void ChooseFolder_Click(object sender, RoutedEventArgs e)
        {
            var picker = new FolderPicker();
            UpdateItem(await picker.PickSingleFolderAsync());
        }

        private void UpdateItem(IStorageItem item)
        {
            selectedItem = item;
            if (selectedItem != null)
            {
                ItemOperationsPanel.Visibility = Visibility.Visible;
                ItemNameBlock.Text = item.Path;
            }
            else
            {
                ItemOperationsPanel.Visibility = Visibility.Collapsed;
            }
        }

        private async void ProtectL1_Click(object sender, RoutedEventArgs e)
        {
            UserDataStorageItemProtectionStatus status = await userDataProtectionManager.ProtectStorageItemAsync(selectedItem, UserDataAvailability.AfterFirstUnlock);
            ReportStatus("Protect L1", status);
        }

        private async void ProtectL2_Click(object sender, RoutedEventArgs e)
        {
            UserDataStorageItemProtectionStatus status = await userDataProtectionManager.ProtectStorageItemAsync(selectedItem, UserDataAvailability.WhileUnlocked);
            ReportStatus("Protect L2", status);
        }

        private async void Unprotect_Click(object sender, RoutedEventArgs e)
        {
            UserDataStorageItemProtectionStatus status = await userDataProtectionManager.ProtectStorageItemAsync(selectedItem, UserDataAvailability.Always);
            ReportStatus("Unprotect", status);
        }

        private void ReportStatus(string operation, UserDataStorageItemProtectionStatus status)
        {
            switch (status)
            {
                case UserDataStorageItemProtectionStatus.Succeeded:
                    rootPage.NotifyUser(operation + " succeeded", NotifyType.StatusMessage);
                    break;

                case UserDataStorageItemProtectionStatus.NotProtectable:
                    rootPage.NotifyUser(operation + " failed: Not protectable", NotifyType.ErrorMessage);
                    break;

                case UserDataStorageItemProtectionStatus.DataUnavailable:
                    rootPage.NotifyUser(operation + " failed: Data unavailable", NotifyType.ErrorMessage);
                    break;

                default:
                    rootPage.NotifyUser(operation + " failed: Unknown failure", NotifyType.ErrorMessage);
                    break;
            }
        }
    }
}
