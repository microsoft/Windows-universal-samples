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
    public sealed partial class S1_AddWithAPI
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public S1_AddWithAPI()
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
            if (ItemKeyInput.Text == "")
            {
                rootPage.NotifyUser("You must add an item key to insert an item into the index.", NotifyType.ErrorMessage);
            }
            else
            {
                // Write the content property to a stream
                var contentStream = new Windows.Storage.Streams.InMemoryRandomAccessStream();
                var contentWriter = new Windows.Storage.Streams.DataWriter(contentStream);
                contentWriter.WriteString(ContentInput.Text);
                await contentWriter.StoreAsync();
                contentStream.Seek(0);

                // Get the name, keywords, and comment properties, and assign a language to them if provided
                object itemNameValue = NameInput.Text;
                object keywordsValue = KeywordsInput.Text;
                object commentValue = CommentInput.Text;
                if (LanguageInput.Text != "")
                {
                    var itemNameValueAndLanguage = new Windows.Storage.Search.ValueAndLanguage();
                    itemNameValueAndLanguage.Language = LanguageInput.Text;
                    itemNameValueAndLanguage.Value = itemNameValue;
                    itemNameValue = itemNameValueAndLanguage;
                    var keywordsValueAndLanguage = new Windows.Storage.Search.ValueAndLanguage();
                    keywordsValueAndLanguage.Language = LanguageInput.Text;
                    keywordsValueAndLanguage.Value = keywordsValue;
                    keywordsValue = keywordsValueAndLanguage;
                    var commentValueAndLanguage = new Windows.Storage.Search.ValueAndLanguage();
                    commentValueAndLanguage.Language = LanguageInput.Text;
                    commentValueAndLanguage.Value = commentValue;
                    commentValue = commentValueAndLanguage;
                }

                // Create the item to add to the indexer
                var content = new Windows.Storage.Search.IndexableContent();
                content.Id = ItemKeyInput.Text;
                content.Properties.Add(Windows.Storage.SystemProperties.ItemNameDisplay, itemNameValue);
                content.Properties.Add(Windows.Storage.SystemProperties.Keywords, keywordsValue);
                content.Properties.Add(Windows.Storage.SystemProperties.Comment, commentValue);
                content.Stream = contentStream;
                content.StreamContentType = "text/plain";

                // Add the item to the indexer
                Helpers.OnIndexerOperationBegin();
                var indexer = Windows.Storage.Search.ContentIndexer.GetIndexer();
                await indexer.AddAsync(content);
                Helpers.OnIndexerOperationComplete(indexer);

                // Retrieve the item from the indexer and output its properties
                var retrievedProperties = await indexer.RetrievePropertiesAsync(ItemKeyInput.Text, content.Properties.Keys);
                rootPage.NotifyUser(Helpers.CreateItemString(ItemKeyInput.Text, content.Properties.Keys, retrievedProperties), NotifyType.StatusMessage);
            }
        }
    }
}
