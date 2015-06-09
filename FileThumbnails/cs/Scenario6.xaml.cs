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
    public sealed partial class Scenario6 : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario6()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage.ResetOutput(ThumbnailImage, OutputTextBlock);
        }

        private async void SelectImageButton_Click(object sender, RoutedEventArgs e)
        {
            rootPage.ResetOutput(ThumbnailImage, OutputTextBlock);

            uint size;
            if (uint.TryParse(RequestSize.Text, out size))
            {
                // Pick a photo
                FileOpenPicker openPicker = new FileOpenPicker();
                foreach (string extension in FileExtensions.Image)
                {
                    openPicker.FileTypeFilter.Add(extension);
                }

                StorageFile file = await openPicker.PickSingleFileAsync();
                if (file != null)
                {
                    const ThumbnailMode thumbnailMode = ThumbnailMode.SingleItem;

                    bool fastThumbnail = FastThumbnailCheckBox.IsChecked.Value;
                    ThumbnailOptions thumbnailOptions = ThumbnailOptions.UseCurrentScale;
                    if (fastThumbnail)
                    {
                        thumbnailOptions |= ThumbnailOptions.ReturnOnlyIfCached;
                    }

                    using (StorageItemThumbnail thumbnail = await file.GetScaledImageAsThumbnailAsync(thumbnailMode, size, thumbnailOptions))
                    {
                        if (thumbnail != null)
                        {
                            MainPage.DisplayResult(ThumbnailImage, OutputTextBlock, thumbnailMode.ToString(), size, file, thumbnail, false);
                        }
                        else if (fastThumbnail)
                        {
                            rootPage.NotifyUser(Errors.NoExifThumbnail, NotifyType.StatusMessage);
                        }
                        else
                        {
                            rootPage.NotifyUser(Errors.NoThumbnail, NotifyType.StatusMessage);
                        }
                    }
                }
                else
                {
                    rootPage.NotifyUser(Errors.Cancel, NotifyType.StatusMessage);
                }
            }
            else
            {
                rootPage.NotifyUser(Errors.InvalidSize, NotifyType.ErrorMessage);
            }
        }
    }
}
