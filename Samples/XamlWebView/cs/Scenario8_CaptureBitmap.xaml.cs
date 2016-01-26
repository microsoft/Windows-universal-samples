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
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario8_CaptureBitmap : Page
    {
        public Scenario8_CaptureBitmap()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            WebViewControl.Navigate(new Uri("http://www.bing.com"));
        }

        /// <summary>
        /// Captures the WebView into a bitmap.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void CaptureButton_Click()
        {
            InMemoryRandomAccessStream stream = new InMemoryRandomAccessStream();
            await WebViewControl.CapturePreviewToStreamAsync(stream);

            // Scale the bitmap to the space available for the thumbnail image,
            // preserving aspect ratio.
            double thumbnailWidth = ThumbnailImage.Width;
            double thumbnailHeight = ThumbnailImage.Height;
            double webViewControlWidth = WebViewControl.ActualWidth;
            double webViewControlHeight = WebViewControl.ActualHeight;
            if (thumbnailWidth == 0 || thumbnailHeight == 0 ||
                webViewControlWidth == 0 || webViewControlHeight == 0)
            {
                // Avoid 0x0 bitmaps, which cause all sorts of problems.
                return;
            }

            double horizontalScale = thumbnailWidth / webViewControlWidth;
            double verticalScale = thumbnailHeight / webViewControlHeight;
            double scale = Math.Min(horizontalScale, verticalScale);
            int width = (int)(webViewControlWidth * scale);
            int height = (int)(webViewControlHeight * scale);
            BitmapSource thumbnailBitmap = await CreateScaledBitmapFromStreamAsync(width, height, stream);

            ThumbnailImage.Source = thumbnailBitmap;
        }

        static async Task<BitmapSource> CreateScaledBitmapFromStreamAsync(int width, int height, IRandomAccessStream source)
        {
            WriteableBitmap bitmap = new WriteableBitmap(width, height);
            BitmapDecoder decoder = await BitmapDecoder.CreateAsync(source);
            BitmapTransform transform = new BitmapTransform();
            transform.ScaledHeight = (uint)height;
            transform.ScaledWidth = (uint)width;
            PixelDataProvider pixelData = await decoder.GetPixelDataAsync(
                BitmapPixelFormat.Bgra8,
                BitmapAlphaMode.Straight,
                transform,
                ExifOrientationMode.RespectExifOrientation,
                ColorManagementMode.DoNotColorManage);
            pixelData.DetachPixelData().CopyTo(bitmap.PixelBuffer);
            return bitmap;
        }
    }
}
