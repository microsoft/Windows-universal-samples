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
using Windows.ApplicationModel.DataTransfer;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media.Imaging;

namespace SDKTemplate
{
    public sealed partial class ShareDelayRenderedFiles
    {
        private StorageFile selectedImage;

        public ShareDelayRenderedFiles()
        {
            this.InitializeComponent();
        }

        protected override bool GetShareContent(DataRequest request)
        {
            bool succeeded = false;

            if (this.selectedImage != null)
            {
                DataPackage requestData = request.Data;
                requestData.Properties.Title = "Delay rendered image";
                requestData.Properties.Description = "Resized image from the Share Source sample";
                requestData.Properties.ContentSourceApplicationLink = ApplicationLink;
                requestData.Properties.Thumbnail = RandomAccessStreamReference.CreateFromFile(this.selectedImage);
                requestData.SetDataProvider(StandardDataFormats.Bitmap, providerRequest => this.OnDeferredImageRequestedHandler(providerRequest, this.selectedImage));
                succeeded = true;
            }
            else
            {
                request.FailWithDisplayText("Select an image you would like to share and try again.");
            }
            return succeeded;
        }

        private async void OnDeferredImageRequestedHandler(DataProviderRequest providerRequest, StorageFile imageFile)
        {
            // In this delegate we provide updated Bitmap data using delayed rendering.

            if (imageFile != null)
            {
                // If the delegate is calling any asynchronous operations it needs to acquire the deferral first. This lets the
                // system know that you are performing some operations that might take a little longer and that the call to
                // SetData could happen after the delegate returns. Once you acquired the deferral object you must call Complete
                // on it after your final call to SetData.
                DataProviderDeferral deferral = providerRequest.GetDeferral();
                InMemoryRandomAccessStream inMemoryStream = new InMemoryRandomAccessStream();

                // Make sure to always call Complete when done with the deferral.
                try
                {
                    // Decode the image and re-encode it at 50% width and height.
                    IRandomAccessStream imageStream = await imageFile.OpenAsync(FileAccessMode.Read);
                    BitmapDecoder imageDecoder = await BitmapDecoder.CreateAsync(imageStream);
                    BitmapEncoder imageEncoder = await BitmapEncoder.CreateForTranscodingAsync(inMemoryStream, imageDecoder);
                    imageEncoder.BitmapTransform.ScaledWidth = (uint)(imageDecoder.OrientedPixelWidth * 0.5);
                    imageEncoder.BitmapTransform.ScaledHeight = (uint)(imageDecoder.OrientedPixelHeight * 0.5);
                    await imageEncoder.FlushAsync();

                    providerRequest.SetData(RandomAccessStreamReference.CreateFromStream(inMemoryStream));
                }
                finally
                {
                    deferral.Complete();
                }
            }
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
                this.selectedImage = pickedImage;

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
