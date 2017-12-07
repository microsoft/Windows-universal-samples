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
using System.IO;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;

namespace SDKTemplate
{
    public sealed partial class Scenario10 : Page
    {
        public Scenario10()
        {
            this.InitializeComponent();

            // Initialize the InkCanvas
            inkCanvas.InkPresenter.InputDeviceTypes =
                Windows.UI.Core.CoreInputDeviceTypes.Mouse |
                Windows.UI.Core.CoreInputDeviceTypes.Pen;
        }

        async Task ColorizeCustomButtonAsync(Windows.UI.Color newColor)
        {
            // Get the bitmap from the png
            var sourceUri = new Uri("ms-appx:///Assets/ButtonIconFill.png");
            StorageFile storageFile = await StorageFile.GetFileFromApplicationUriAsync(sourceUri);
            using (IRandomAccessStream readableStream = await storageFile.OpenAsync(FileAccessMode.Read))
            {
                BitmapDecoder bitmapDecoder = await BitmapDecoder.CreateAsync(readableStream);

                // Get the raw pixel data from the bitmap
                var dummyTransform = new BitmapTransform();
                PixelDataProvider pixelDataProvider = await bitmapDecoder.GetPixelDataAsync(
                    BitmapPixelFormat.Bgra8,
                    BitmapAlphaMode.Straight,
                    dummyTransform,
                    ExifOrientationMode.RespectExifOrientation,
                    ColorManagementMode.DoNotColorManage);

                byte[] pixelData = pixelDataProvider.DetachPixelData();

                // We now have direct access to the pixels.
                for (uint row = 0; row < bitmapDecoder.PixelHeight; row++)
                {
                    for (uint col = 0; col < bitmapDecoder.PixelWidth; col++)
                    {
                        var offset = (row * (int)bitmapDecoder.PixelWidth * 4) + (col * 4);

                        // Replace the pixel with newColor if it is our color key.
                        if ((pixelData[offset] <= 0x01) &&
                            (pixelData[offset + 1] <= 0x01) &&
                            (pixelData[offset + 2] <= 0x01) &&
                            (pixelData[offset + 3] != 0x00))
                        {
                            pixelData[offset] = newColor.B;
                            pixelData[offset + 1] = newColor.G;
                            pixelData[offset + 2] = newColor.R;
                            pixelData[offset + 3] = newColor.A;
                        }
                    }
                }

                // Create a bitmap from our modified pixel data.
                var bitmap = new WriteableBitmap((int)bitmapDecoder.PixelWidth, (int)bitmapDecoder.PixelHeight);

                using (Stream pixelStream = bitmap.PixelBuffer.AsStream())
                {
                    await pixelStream.WriteAsync(pixelData, 0, pixelData.Length);
                    pixelStream.Flush();
                    bitmap.Invalidate();
                }

                CustomButtonFill.Source = bitmap;
            }
        }

        private async void inkToolbar_InkDrawingAttributesChanged(InkToolbar sender, object args)
        {
            // Change the color of the button icon for the calligraphic pen button
            if (CalligraphicPenButton == inkToolbar.ActiveTool)
            {
                await ColorizeCustomButtonAsync(sender.InkDrawingAttributes.Color);
            }
        }

        private async void inkToolbar_Loaded(object sender, RoutedEventArgs e)
        {
            var selectedBrush = (SolidColorBrush)CalligraphicPenButton.SelectedBrush;
            await ColorizeCustomButtonAsync(selectedBrush.Color);
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            HelperFunctions.UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
        }
    }
}
