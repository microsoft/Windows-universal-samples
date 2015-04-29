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
using Windows.Foundation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class S2_UpdateAndDeleteWithAPI
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public S2_UpdateAndDeleteWithAPI()
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

        private async void UpdatePropertyHelper(string itemKey, string propertyKey, string propertyName, object newValue)
        {
            var indexer = Windows.Storage.Search.ContentIndexer.GetIndexer();
            string[] propertiesToRetrieve = { propertyKey };

            try
            {
                // Log the original value of the property
                var retrievedProperties = await indexer.RetrievePropertiesAsync(itemKey, propertiesToRetrieve);
                var outputString = "Original " + propertyName + ": " + Helpers.StringifyProperty(retrievedProperties[propertyKey]);

                // Create an indexable content item to update the property
                var content = new Windows.Storage.Search.IndexableContent();
                content.Properties.Add(propertyKey, newValue);
                content.Id = itemKey;

                // Update the indexer
                Helpers.OnIndexerOperationBegin();
                await indexer.UpdateAsync(content);
                Helpers.OnIndexerOperationComplete(indexer);

                // Log the new value of the property
                retrievedProperties = await indexer.RetrievePropertiesAsync(itemKey, propertiesToRetrieve);
                outputString += "\nNew " + propertyName + ": " + Helpers.StringifyProperty(retrievedProperties[propertyKey]);
                rootPage.NotifyUser(outputString, NotifyType.StatusMessage);
            }
            catch (Exception err)
            {
                rootPage.NotifyUser("Exception thrown! Did you try to retrieve or update a property on a non-existent item?" +
                                    "\nMessage: " + err.Message +
                                    "\nHRESULT: " + String.Format("{0,8:X}", err.HResult),
                                    NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This function updates the ItemNameDisplay property for the indexed item with the key "Sample 0." The
        /// updated ItemNameDisplay property comes from the itemNameInput text box.
        /// </summary>
        private void UpdateName_Click(object sender, RoutedEventArgs e)
        {
            UpdatePropertyHelper("SampleKey0", Windows.Storage.SystemProperties.ItemNameDisplay, "item name", ItemNameInput.Text);
        }

        /// <summary>
        /// This function deletes the Keywords property for the indexed item with the key "Sample0."
        /// </summary>
        private void DeleteKeywords_Click(object sender, RoutedEventArgs e)
        {
            UpdatePropertyHelper("SampleKey0", Windows.Storage.SystemProperties.Keywords, "keywords", null);
        }

        delegate IAsyncAction DeleteFunction(Windows.Storage.Search.ContentIndexer indexer);

        private async void DeleteItemsHelper(DeleteFunction deleteFunction)
        {
            var indexer = Windows.Storage.Search.ContentIndexer.GetIndexer();

            // Log the original number of items
            string[] propertiesToRetrieve = { Windows.Storage.SystemProperties.ItemNameDisplay };
            var query = indexer.CreateQuery("*", propertiesToRetrieve);
            uint count = await query.GetCountAsync();
            var outputString = "Original number of items: " + count;
            Helpers.OnIndexerOperationBegin();

            // Call the deletion function
            await deleteFunction(indexer);

            // Log the new number of items
            Helpers.OnIndexerOperationComplete(indexer);
            query = indexer.CreateQuery("*", propertiesToRetrieve);
            count = await query.GetCountAsync();
            outputString += "\nNew number of items: " + count;
            rootPage.NotifyUser(outputString, NotifyType.StatusMessage);
        }

        /// <summary>
        /// This function deletes the indexed item with the key "SampleKey0" added to the index by this app via the
        /// ContentIndexer
        /// </summary>
        private void DeleteSingleItem_Click(object sender, RoutedEventArgs e)
        {
            DeleteItemsHelper(indexer => indexer.DeleteAsync("SampleKey0"));
        }

        /// <summary>
        /// This function deletes the indexed items with the keys "SampleKey1" and "SampleKey2" added to the index
        /// by this app via the ContentIndexer
        /// </summary>
        private void DeleteMultipleItems_Click(object sender, RoutedEventArgs e)
        {
            string[] keys = { "SampleKey1", "SampleKey2" };
            DeleteItemsHelper(indexer => indexer.DeleteMultipleAsync(keys));
        }

        /// <summary>
        /// This function deletes all items added to the index by this app via the ContentIndexer
        /// </summary>
        private void DeleteAllItems_Click(object sender, RoutedEventArgs e)
        {
            DeleteItemsHelper(indexer => indexer.DeleteAllAsync());
        }
    }
}
