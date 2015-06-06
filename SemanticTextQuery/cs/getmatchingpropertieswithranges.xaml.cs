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

using System.Collections.Generic;
using Windows.Foundation;
using Windows.Storage;
using Windows.Storage.Search;
using Windows.UI.Text;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using System;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario3
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario3()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            FindQueryText.Text = "a AND datemodified:>2/1/2013";
        }

        private async void Find_Click(object sender, RoutedEventArgs e)
        {
            ContentTextOutput.Text = "";
            if (FindQueryText.Text == "")
            {
                return;
            }

            List<string> propertyNames = new List<string>();
            propertyNames.Add("System.FileName");
            var queryOptions = new Windows.Storage.Search.QueryOptions();
            queryOptions.IndexerOption = Windows.Storage.Search.IndexerOption.OnlyUseIndexer;
            queryOptions.UserSearchFilter = FindQueryText.Text;
            queryOptions.SetPropertyPrefetch(Windows.Storage.FileProperties.PropertyPrefetchOptions.DocumentProperties, propertyNames);

            // Query the Pictures library.
            StorageFileQueryResult queryResult = Windows.Storage.KnownFolders.PicturesLibrary.CreateFileQueryWithOptions(queryOptions);
            IReadOnlyList<StorageFile> files = await queryResult.GetFilesAsync();
            foreach (StorageFile file in files)
            {
                IDictionary<String, IReadOnlyList<Windows.Data.Text.TextSegment>> fileRangeProperties = queryResult.GetMatchingPropertiesWithRanges(file);
                if (fileRangeProperties.ContainsKey("System.FileName"))
                {
                    IReadOnlyList<Windows.Data.Text.TextSegment> ranges;
                    fileRangeProperties.TryGetValue("System.FileName", out ranges);
                    rootPage.HighlightRanges(ContentTextOutput, file.DisplayName, ranges);
                }
                // Note: You can continue looking for other properties you would like to highlight on the file here.
            }
            if (files.Count == 0)
            {
                ContentTextOutput.Text = "There were no matching files in your Pictures Library";
            }
        }
    }
}
