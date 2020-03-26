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
using System.Threading.Tasks;
using Windows.ApplicationModel.DataTransfer;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

namespace SDKTemplate
{
    public sealed partial class CopyImage : Page
    {
        MainPage rootPage = MainPage.Current;

        public CopyImage()
        {
            this.InitializeComponent();
        }

        void CopyButton_Click(object sender, RoutedEventArgs e)
        {
            this.CopyBitmap(false);
        }

        void CopyWithDelayedRenderingButton_Click(object sender, RoutedEventArgs e)
        {
            this.CopyBitmap(true);
        }

        async void PasteButton_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            // Get the bitmap and display it.
            var dataPackageView = Clipboard.GetContent();
            if (dataPackageView.Contains(StandardDataFormats.Bitmap))
            {
                IRandomAccessStreamReference imageReceived = null;
                try
                {
                    imageReceived = await dataPackageView.GetBitmapAsync();
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser("Error retrieving image from Clipboard: " + ex.Message, NotifyType.ErrorMessage);
                }

                if (imageReceived != null)
                {
                    using (var imageStream = await imageReceived.OpenReadAsync())
                    {
                        var bitmapImage = new BitmapImage();
                        bitmapImage.SetSource(imageStream);
                        OutputImage.Source = bitmapImage;
                        OutputImage.Visibility = Visibility.Visible;
                        OutputText.Text = "Image is retrieved from the clipboard and pasted successfully.";
                    }
                }
            }
            else
            {
                OutputText.Text = "Bitmap format is not available in clipboard";
                OutputImage.Visibility = Visibility.Collapsed;
            }
        }

        async private static void OnDeferredImageRequestedHandler(DataProviderRequest request, StorageFile imageFile)
        {
            // Since this method is using "await" prior to setting the data in DataPackage,
            // deferral object must be used
            var deferral = request.GetDeferral();

            // Use try/finally to ensure that we always complete the deferral.
            try
            {
                using (var imageStream = await imageFile.OpenAsync(FileAccessMode.Read))
                {
                    // Decode the image
                    var imageDecoder = await BitmapDecoder.CreateAsync(imageStream);

                    // Re-encode the image at 50% width and height
                    var inMemoryStream = new InMemoryRandomAccessStream();
                    var imageEncoder = await BitmapEncoder.CreateForTranscodingAsync(inMemoryStream, imageDecoder);
                    imageEncoder.BitmapTransform.ScaledWidth = (uint)(imageDecoder.OrientedPixelWidth * 0.5);
                    imageEncoder.BitmapTransform.ScaledHeight = (uint)(imageDecoder.OrientedPixelHeight * 0.5);
                    await imageEncoder.FlushAsync();

                    request.SetData(RandomAccessStreamReference.CreateFromStream(inMemoryStream));
                }
            }
            finally
            {
                deferral.Complete();
            }

            MainPage.DisplayToast("Image has been set via deferral.");
        }

        async private void CopyBitmap(bool isDelayRendered)
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            var imagePicker = new FileOpenPicker
            {
                ViewMode = PickerViewMode.Thumbnail,
                SuggestedStartLocation = PickerLocationId.PicturesLibrary,
                FileTypeFilter = { ".jpg", ".png", ".bmp", ".gif", ".tif" }
            };

            var imageFile = await imagePicker.PickSingleFileAsync();
            if (imageFile != null)
            {
                var dataPackage = new DataPackage();

                string message;
                if (isDelayRendered)
                {
                    dataPackage.SetDataProvider(StandardDataFormats.Bitmap, request => OnDeferredImageRequestedHandler(request, imageFile));
                    message = "Image has been copied using delayed rendering";
                }
                else
                {
                    dataPackage.SetBitmap(RandomAccessStreamReference.CreateFromFile(imageFile));
                    message = "Image has been copied";
                }

                if (Clipboard.SetContentWithOptions(dataPackage, null))
                {
                    rootPage.NotifyUser(message, NotifyType.StatusMessage);
                }
                else
                {
                    // Copying data to Clipboard can potentially fail - for example, if another application is holding Clipboard open
                    rootPage.NotifyUser("Error copying content to clipboard.", NotifyType.ErrorMessage);
                }
            }
        }
    }
}
