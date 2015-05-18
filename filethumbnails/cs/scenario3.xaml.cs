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
    public sealed partial class Scenario3 : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario3()
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

            // Pick a document
            FileOpenPicker openPicker = new FileOpenPicker();
            foreach (string extension in FileExtensions.Document)
            {
                openPicker.FileTypeFilter.Add(extension);
            }

            StorageFile file = await openPicker.PickSingleFileAsync();
            if (file != null)
            {
                const ThumbnailMode thumbnailMode = ThumbnailMode.DocumentsView;
                const uint size = 100;
                using (StorageItemThumbnail thumbnail = await file.GetThumbnailAsync(thumbnailMode, size))
                {
                    if (thumbnail != null)
                    {
                        MainPage.DisplayResult(ThumbnailImage, OutputTextBlock, thumbnailMode.ToString(), size, file, thumbnail, false);
                    }
                    else
                    {
                        rootPage.NotifyUser(Errors.NoIcon, NotifyType.StatusMessage);
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
