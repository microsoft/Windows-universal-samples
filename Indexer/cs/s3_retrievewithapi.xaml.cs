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
using System.Collections.Generic;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class S3_RetrieveWithAPI
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public S3_RetrieveWithAPI()
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
            var indexer = Windows.Storage.Search.ContentIndexer.GetIndexer();
            string outputString = await Helpers.AddItemsToIndex(indexer);
            rootPage.NotifyUser(outputString, NotifyType.StatusMessage);
        }

        private async void ExecuteQueryHelper(string queryString)
        {
            var indexer = Windows.Storage.Search.ContentIndexer.GetIndexer();
            string[] propertyKeys =
            {
                Windows.Storage.SystemProperties.ItemNameDisplay,
                Windows.Storage.SystemProperties.Keywords,
                Windows.Storage.SystemProperties.Comment
            };
            var query = indexer.CreateQuery(queryString, propertyKeys);
            var documents = await query.GetAsync();
            string outputString = "";
            foreach (var document in documents)
            {
                outputString += Helpers.CreateItemString(document.Id, propertyKeys, document.Properties) + "\n";
            }
            rootPage.NotifyUser(outputString, NotifyType.StatusMessage);
        }

        /// <summary>
        /// This function retrieves all items added to the index by this app via the ContentIndexer.
        /// </summary>
        private void RetrieveAllItems_Click(object sender, RoutedEventArgs e)
        {
            ExecuteQueryHelper("*");
        }

        /// <summary>
        /// This function retrieves all items added to the index by this app via the ContentIndexer which contain the work "0" in
        /// either the ItemNameDisplay, Keywords, or Comment properties.
        /// </summary>
        private void RetrieveMatchingItems_Click(object sender, RoutedEventArgs e)
        {
            ExecuteQueryHelper("0");
        }
    }
}
