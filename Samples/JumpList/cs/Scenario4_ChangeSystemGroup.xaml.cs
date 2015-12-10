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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.StartScreen;
using Windows.Storage;
using Windows.Storage.AccessCache;

namespace SDKTemplate
{
    public sealed partial class Scenario4_ChangeSystemGroup : Page
    {
        public Scenario4_ChangeSystemGroup()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            var jumpList = await JumpList.LoadCurrentAsync();
            switch(jumpList.SystemGroupKind)
            {
                case JumpListSystemGroupKind.Recent:
                    SystemGroup_Recent.IsChecked = true;
                    break;
                case JumpListSystemGroupKind.Frequent:
                    SystemGroup_Frequent.IsChecked = true;
                    break;
                case JumpListSystemGroupKind.None:
                    SystemGroup_None.IsChecked = true;
                    break;
            }
        }

        private async void SystemGroup_Recent_Click(object sender, RoutedEventArgs e)
        {
            var jumpList = await JumpList.LoadCurrentAsync();
            jumpList.SystemGroupKind = JumpListSystemGroupKind.Recent;
            await jumpList.SaveAsync();
        }

        private async void SystemGroup_Frequent_Click(object sender, RoutedEventArgs e)
        {
            var jumpList = await JumpList.LoadCurrentAsync();
            jumpList.SystemGroupKind = JumpListSystemGroupKind.Frequent;
            await jumpList.SaveAsync();
        }

        private async void SystemGroup_None_Click(object sender, RoutedEventArgs e)
        {
            var jumpList = await JumpList.LoadCurrentAsync();
            jumpList.SystemGroupKind = JumpListSystemGroupKind.None;
            await jumpList.SaveAsync();
        }

        private async void PrepareSampleFiles_Click(object sender, RoutedEventArgs e)
        {
            // NOTE: This is for the sake of the sample only. Real apps should allow
            // the system list to be populated by user activity.

            for (int i = 0; i < 5; ++i)
            {
                string fileName = "Temp" + i + ".ms-jumplist-sample";
                var localFolder = ApplicationData.Current.LocalFolder;
                var file = await localFolder.TryGetItemAsync(fileName);
                if (file == null)
                {
                    file = await localFolder.CreateFileAsync(fileName);
                    StorageApplicationPermissions.MostRecentlyUsedList.Add(file, "", RecentStorageItemVisibility.AppAndSystem);
                }
            }
        }
    }
}
