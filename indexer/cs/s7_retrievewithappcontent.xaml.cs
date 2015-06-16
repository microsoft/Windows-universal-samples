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
    public sealed partial class S7_RetrieveWithAppContent
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public S7_RetrieveWithAppContent()
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
        
        delegate Windows.Storage.Search.StorageFileQueryResult GetQueryDelegate(Windows.Storage.StorageFolder folder);

        private async void RetrieveAppContentHelper(GetQueryDelegate getQuery)
        {
            var indexedFolder = await Windows.Storage.ApplicationData.Current.LocalFolder.CreateFolderAsync("Indexed", Windows.Storage.CreationCollisionOption.OpenIfExists);
            var query = getQuery(indexedFolder);
            var files = await query.GetFilesAsync();
            string[] propertyKeys =
            {
                Windows.Storage.SystemProperties.ItemNameDisplay,
                Windows.Storage.SystemProperties.Keywords,
                Windows.Storage.SystemProperties.Comment,
                Windows.Storage.SystemProperties.Title
            };
            var outputString = "";
            foreach (var file in files)
            {
                var retrievedProperties = await file.Properties.RetrievePropertiesAsync(propertyKeys);
                outputString += "Name: " + Helpers.StringifyProperty(retrievedProperties[Windows.Storage.SystemProperties.ItemNameDisplay]) + "\n";
                outputString += "Keywords: " + Helpers.StringifyProperty(retrievedProperties[Windows.Storage.SystemProperties.Keywords]) + "\n";
                outputString += "Comment: " + Helpers.StringifyProperty(retrievedProperties[Windows.Storage.SystemProperties.Comment]) + "\n";
                outputString += "Title: " + Helpers.StringifyProperty(retrievedProperties[Windows.Storage.SystemProperties.Title]) + "\n\n";
            }
            rootPage.NotifyUser(outputString, NotifyType.StatusMessage);
        }

        /// <summary>
        /// This function retrieves all items added to the index through the app's appcontent-ms files
        /// </summary>
        private void RetrieveAllItems_Click(object sender, RoutedEventArgs e)
        {
            // Query for all files in the "LocalState\Indexed" folder and sort the results by name
            RetrieveAppContentHelper(folder => folder.CreateFileQuery(Windows.Storage.Search.CommonFileQuery.OrderByName));
        }

        /// <summary>
        /// This function retrieves all items added to the index through the app's appcontent-ms files which
        /// have an ItemNameDisplay property containing "Sample 1"
        /// </summary>
        private void RetrieveMatchedItems_Click(object sender, RoutedEventArgs e)
        {
            var queryOptions = new Windows.Storage.Search.QueryOptions();
            // Include only items returned via the index in the search results
            queryOptions.IndexerOption = Windows.Storage.Search.IndexerOption.OnlyUseIndexer;
            // Create an AQS (Advanced Query Syntax) query which will look for ItemNameDisplay properties which contain "Sample 1"
            queryOptions.ApplicationSearchFilter = Windows.Storage.SystemProperties.ItemNameDisplay + ":\"Sample 1\"";
            RetrieveAppContentHelper(folder => folder.CreateFileQueryWithOptions(queryOptions));
        }
    }
}
