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
using Windows.Storage.FileProperties;
using Windows.Storage.Pickers;
using Windows.Storage.Search;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario5 : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario5()
        {
            this.InitializeComponent();
            GetThumbnailButton.Click += new RoutedEventHandler(GetThumbnailButton_Click);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage.ResetOutput(ThumbnailImage, OutputTextBlock, OutputDetails);
        }

        private async void GetThumbnailButton_Click(object sender, RoutedEventArgs e)
        {
            rootPage.ResetOutput(ThumbnailImage, OutputTextBlock, OutputDetails);

            // Pick a folder
            FolderPicker folderPicker = new FolderPicker();
            foreach (string extension in FileExtensions.Image)
            {
                folderPicker.FileTypeFilter.Add(extension);
            }

            StorageFolder folder = await folderPicker.PickSingleFolderAsync();
            if (folder != null)
            {
                const CommonFolderQuery monthShape = CommonFolderQuery.GroupByMonth;
                // Verify queries are supported because they are not supported in all picked locations.
                if (folder.IsCommonFolderQuerySupported(monthShape))
                {
                    // Convert folder to file group and query for items
                    IReadOnlyList<StorageFolder> months = await folder.CreateFolderQuery(monthShape).GetFoldersAsync();

                    if (months != null && months.Count > 0)
                    {
                        const ThumbnailMode thumbnailMode = ThumbnailMode.PicturesView;
                        const uint size = 200;
                        StorageFolder firstMonth = months[0];
                        using (StorageItemThumbnail thumbnail = await firstMonth.GetThumbnailAsync(thumbnailMode, size))
                        {
                            if (thumbnail != null)
                            {
                                MainPage.DisplayResult(ThumbnailImage, OutputTextBlock, thumbnailMode.ToString(), size, firstMonth, thumbnail, true);

                                // Also display the hierarchy of the file group to better visualize where the thumbnail comes from
                                IReadOnlyList<StorageFile> files = await firstMonth.GetFilesAsync();

                                if (files != null)
                                {
                                    StringBuilder output = new StringBuilder("\nList of files in this group:\n\n");
                                    foreach (StorageFile file in files)
                                    {
                                        output.AppendFormat("{0}\n", file.Name);
                                    }
                                    OutputDetails.Text = output.ToString();
                                }
                            }
                            else
                            {
                                rootPage.NotifyUser(Errors.NoImages, NotifyType.StatusMessage);
                            }
                        }
                    }
                    else
                    {
                        rootPage.NotifyUser(Errors.FileGroupEmpty, NotifyType.StatusMessage);
                    }
                }
                else
                {
                    rootPage.NotifyUser(Errors.FileGroupLocation, NotifyType.StatusMessage);
                }
            }
            else
            {
                rootPage.NotifyUser(Errors.Cancel, NotifyType.StatusMessage);
            }
        }
    }
}
