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
using System.Text;
using Windows.Storage;
using Windows.Storage.Search;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        public Scenario1()
        {
            this.InitializeComponent();
            SearchButton.Click += new RoutedEventHandler(SearchButton_Click);
        }

        private async void SearchButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFolder musicFolder = await KnownFolders.GetFolderForUserAsync(null /* current user */, KnownFolderId.MusicLibrary);
            
            List<string> fileTypeFilter = new List<string>();
            fileTypeFilter.Add("*");

            QueryOptions queryOptions = new QueryOptions(CommonFileQuery.OrderBySearchRank, fileTypeFilter);
            //use the user's input to make a query
            queryOptions.UserSearchFilter = InputTextBox.Text;
            StorageFileQueryResult queryResult = musicFolder.CreateFileQueryWithOptions(queryOptions);

            StringBuilder outputText = new StringBuilder();

            //find all files that match the query
            IReadOnlyList<StorageFile> files = await queryResult.GetFilesAsync();
            //output how many files that match the query were found
            if (files.Count == 0)
            {
                outputText.Append("No files found for '" + queryOptions.UserSearchFilter + "'");
            }
            else if (files.Count == 1)
            {
                outputText.Append(files.Count + " file found:\n\n");
            }
            else
            {
                outputText.Append(files.Count + " files found:\n\n");
            }

            //output the name of each file that matches the query
            foreach (StorageFile file in files)
            {
                outputText.Append(file.Name + "\n");
            }

            OutputTextBlock.Text = outputText.ToString();
        }
    }
}
