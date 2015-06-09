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
using Windows.Storage;
using Windows.Storage.FileProperties;
using Windows.Storage.Pickers;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario4 : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario4()
        {
            this.InitializeComponent();
            GetThumbnailButton.Click += new RoutedEventHandler(GetThumbnailButton_Click);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage.ResetOutput(ThumbnailImage, OutputTextBlock);
        }

        private async void GetThumbnailButton_Click(object sender, RoutedEventArgs e)
        {
            rootPage.ResetOutput(ThumbnailImage, OutputTextBlock);

            // Pick a folder
            FolderPicker folderPicker = new FolderPicker();
            foreach (string extension in FileExtensions.Image)
            {
                folderPicker.FileTypeFilter.Add(extension);
            }

            StorageFolder folder = await folderPicker.PickSingleFolderAsync();
            if (folder != null)
            {
                const ThumbnailMode thumbnailMode = ThumbnailMode.PicturesView;
                const uint size = 200;
                using (StorageItemThumbnail thumbnail = await folder.GetThumbnailAsync(thumbnailMode, size))
                {
                    if (thumbnail != null)
                    {
                        MainPage.DisplayResult(ThumbnailImage, OutputTextBlock, thumbnailMode.ToString(), size, folder, thumbnail, false);
                    }
                    else
                    {
                        rootPage.NotifyUser(Errors.NoImages, NotifyType.StatusMessage);
                    }
                }
            }
            else
            {
                rootPage.NotifyUser(Errors.Cancel, NotifyType.StatusMessage);
            }
        }
    }
}
