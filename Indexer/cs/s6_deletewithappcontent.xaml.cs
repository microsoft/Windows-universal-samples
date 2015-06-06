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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using System;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class S6_DeleteWithAppContent
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public S6_DeleteWithAppContent()
        {
            this.InitializeComponent();
            Helpers.InitializeRevisionNumber();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }

        private async void AddToIndex_Click(object sender, RoutedEventArgs e)
        {
            var outputString = await Helpers.AddAppContentFilesToIndexedFolder();
            rootPage.NotifyUser(outputString, NotifyType.StatusMessage);
        }

        /// <summary>
        /// This function deletes a single appcontent-ms file, "sample1.appcontent-ms". Upon deletion, the indexer is notified
        /// and the content indexed from this file is removed from the index.
        /// </summary>
        private async void DeleteSingleItem_Click(object sender, RoutedEventArgs e)
        {
            var localFolder = Windows.Storage.ApplicationData.Current.LocalFolder;
            var indexedFolder = await localFolder.CreateFolderAsync("Indexed", Windows.Storage.CreationCollisionOption.OpenIfExists);
            try
            {
                var file = await indexedFolder.GetFileAsync("sample1.appcontent-ms");
                await file.DeleteAsync();
                rootPage.NotifyUser("Items deleted from the \"Indexed\" folder: " + file.DisplayName, NotifyType.StatusMessage);
            }
            catch (Exception err)
            {
                rootPage.NotifyUser("Exception thrown! Did you already delete the \"sample1.appcontent-ms\" file?" +
                                    "\nMessage: " + err.Message +
                                    "\nHRESULT: " + String.Format("{0,8:X}", err.HResult),
                                    NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This function deletes all appcontent-ms files in the "LocalState\Indexed" folder. Upon deletion, the indexer is notified and
        /// the content indexed from these files is removed from the index.
        /// </summary>
        private async void DeleteAllItems_Click(object sender, RoutedEventArgs e)
        {
            var localFolder = Windows.Storage.ApplicationData.Current.LocalFolder;
            var indexedFolder = await localFolder.CreateFolderAsync("Indexed", Windows.Storage.CreationCollisionOption.OpenIfExists);
            var files = await indexedFolder.GetFilesAsync();
            var outputString = "Items deleted from the \"Indexed\" folder:";
            foreach (var file in files)
            {
                outputString += "\n" + file.DisplayName;
                await file.DeleteAsync();
            }
            rootPage.NotifyUser(outputString, NotifyType.StatusMessage);
        }
    }
}
