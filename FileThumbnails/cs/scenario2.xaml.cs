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
    public sealed partial class Scenario2 : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario2()
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

            // Pick a music file
            FileOpenPicker openPicker = new FileOpenPicker();
            foreach (string extension in FileExtensions.Music)
            {
                openPicker.FileTypeFilter.Add(extension);
            }

            StorageFile file = await openPicker.PickSingleFileAsync();

            if (file != null)
            {
                const ThumbnailMode thumbnailMode = ThumbnailMode.MusicView;
                const uint size = 100;
                using (StorageItemThumbnail thumbnail = await file.GetThumbnailAsync(thumbnailMode, size))
                {
                    // Also verify the type is ThumbnailType.Image (album art) instead of ThumbnailType.Icon
                    // (which may be returned as a fallback if the file does not provide album art)
                    if (thumbnail != null && thumbnail.Type == ThumbnailType.Image)
                    {
                        // Display the thumbnail
                        MainPage.DisplayResult(ThumbnailImage, OutputTextBlock, thumbnailMode.ToString(), size, file, thumbnail, false);
                    }
                    else
                    {
                        rootPage.NotifyUser(Errors.NoAlbumArt, NotifyType.StatusMessage);
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
