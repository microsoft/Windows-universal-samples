//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.Search;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2 : Page
    {
        public Scenario2()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// list all the files and folders in Pictures library by month
        /// </summary>
        private async void GroupByMonthButton_Click(object sender, RoutedEventArgs e)
        {
            await GroupByHelperAsync(new QueryOptions(CommonFolderQuery.GroupByMonth));
        }

        /// <summary>
        /// list all the files and folders in Pictures library by rating
        /// </summary>
        private async void GroupByRatingButton_Click(object sender, RoutedEventArgs e)
        {
            await GroupByHelperAsync(new QueryOptions(CommonFolderQuery.GroupByRating));
        }

        /// <summary>
        /// list all the files and folders in Pictures library by tag
        /// </summary>
        private async void GroupByTagButton_Click(object sender, RoutedEventArgs e)
        {
            await GroupByHelperAsync(new QueryOptions(CommonFolderQuery.GroupByTag));
        }

        /// <summary>
        /// helper for all list by functions
        /// </summary>
        async Task GroupByHelperAsync(QueryOptions queryOptions)
        {
            OutputPanel.Children.Clear();

            StorageFolder picturesFolder = await KnownFolders.GetFolderForUserAsync(null /* current user */, KnownFolderId.PicturesLibrary);
            StorageFolderQueryResult queryResult = picturesFolder.CreateFolderQueryWithOptions(queryOptions);

            IReadOnlyList<StorageFolder> folderList = await queryResult.GetFoldersAsync();
            foreach (StorageFolder folder in folderList)
            {
                IReadOnlyList<StorageFile> fileList = await folder.GetFilesAsync();
                OutputPanel.Children.Add(CreateHeaderTextBlock(folder.Name + " (" + fileList.Count + ")"));
                foreach (StorageFile file in fileList)
                {
                    OutputPanel.Children.Add(CreateLineItemTextBlock(file.Name));
                }
            }
        }

        TextBlock CreateHeaderTextBlock(string contents)
        {
            TextBlock textBlock = new TextBlock();
            textBlock.Text = contents;
            textBlock.Style = (Style)Application.Current.Resources["SampleHeaderTextStyle"];
            textBlock.TextWrapping = TextWrapping.Wrap;
            return textBlock;
        }

        TextBlock CreateLineItemTextBlock(string contents)
        {
            TextBlock textBlock = new TextBlock();
            textBlock.Text = contents;
            textBlock.Style = (Style)Application.Current.Resources["BasicTextStyle"];
            textBlock.TextWrapping = TextWrapping.Wrap;
            Thickness margin = textBlock.Margin;
            margin.Left = 20;
            textBlock.Margin = margin;
            return textBlock;
        }
    }
}
