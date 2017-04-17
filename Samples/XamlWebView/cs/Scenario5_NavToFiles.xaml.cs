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

using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using System;

namespace SDKTemplate
{
    public sealed partial class Scenario5_NavToFiles : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario5_NavToFiles()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Navigates the webview to the application package
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void LoadFromPackage_Click()
        {
            string url = "ms-appx-web:///html/html_example2.html";
            WebViewControl.Navigate(new Uri(url));
            rootPage.NotifyUser($"Navigated to {url}", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Navigates the webview to the local state directory
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void LoadFromLocalState_Click()
        {
            // Create a file in a subdirectory.
            // The ms-appdata protocol isolates top-level subdirectories from each other.

            StorageFolder stateFolder = await ApplicationData.Current.LocalFolder.CreateFolderAsync("NavigateToState", CreationCollisionOption.OpenIfExists);
            StorageFile htmlFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri($"ms-appx:///html/html_example2.html"));

            await htmlFile.CopyAsync(stateFolder, "test.html", NameCollisionOption.ReplaceExisting);

            string url = "ms-appdata:///local/NavigateToState/test.html";
            WebViewControl.Navigate(new Uri(url));
            rootPage.NotifyUser($"Navigated to {url}", NotifyType.StatusMessage);
        }
    }
}
