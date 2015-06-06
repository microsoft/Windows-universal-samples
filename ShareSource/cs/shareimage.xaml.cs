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
using Windows.ApplicationModel.DataTransfer;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media.Imaging;

namespace SDKTemplate
{
    public sealed partial class ShareImage
    {
        private StorageFile imageFile;

        public ShareImage()
        {
            this.InitializeComponent();
        }

        protected override bool GetShareContent(DataRequest request)
        {
            bool succeeded = false;

            if (this.imageFile != null)
            {
                DataPackage requestData = request.Data;
                requestData.Properties.Title = TitleInputBox.Text;
                requestData.Properties.Description = DescriptionInputBox.Text; // The description is optional.
                requestData.Properties.ContentSourceApplicationLink = ApplicationLink;

                // It's recommended to use both SetBitmap and SetStorageItems for sharing a single image
                // since the target app may only support one or the other.

                List<IStorageItem> imageItems = new List<IStorageItem>();
                imageItems.Add(this.imageFile);
                requestData.SetStorageItems(imageItems);

                RandomAccessStreamReference imageStreamRef = RandomAccessStreamReference.CreateFromFile(this.imageFile);
                requestData.Properties.Thumbnail = imageStreamRef;
                requestData.SetBitmap(imageStreamRef);
                succeeded = true;
            }
            else
            {
                request.FailWithDisplayText("Select an image you would like to share and try again.");
            }
            return succeeded;
        }

        private async void SelectImageButton_Click(object sender, RoutedEventArgs e)
        {
            FileOpenPicker imagePicker = new FileOpenPicker
            {
                ViewMode = PickerViewMode.Thumbnail,
                SuggestedStartLocation = PickerLocationId.PicturesLibrary,
                FileTypeFilter = { ".jpg", ".png", ".bmp", ".gif", ".tif" }
            };

            StorageFile pickedImage = await imagePicker.PickSingleFileAsync();

            if (pickedImage != null)
            {
                this.imageFile = pickedImage;

                // Display the image in the UI.
                IRandomAccessStream displayStream = await pickedImage.OpenAsync(FileAccessMode.Read);
                BitmapImage bitmapImage = new BitmapImage();
                bitmapImage.SetSource(displayStream);
                ImageHolder.Source = bitmapImage;
                this.rootPage.NotifyUser("Selected " + pickedImage.Name + ".", NotifyType.StatusMessage);

                ShareStep.Visibility = Visibility.Visible;
            }
        }
    }
}
