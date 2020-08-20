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

using Windows.Graphics.Imaging;
using Windows.Media.FaceAnalysis;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Page for demonstrating FaceDetection on an image file.
    /// </summary>
    public sealed partial class Scenario1_DetectInPhoto : Page
    {
        /// <summary>
        /// Brush for drawing the bounding box around each detected face.
        /// </summary>
        private readonly SolidColorBrush lineBrush = new SolidColorBrush(Windows.UI.Colors.Yellow);

        /// <summary>
        /// Thickness of the face bounding box lines.
        /// </summary>
        private readonly double lineThickness = 2.0;

        /// <summary>
        /// Transparent fill for the bounding box.
        /// </summary>
        private readonly SolidColorBrush fillBrush = new SolidColorBrush(Windows.UI.Colors.Transparent);

        /// <summary>
        /// Limit on the height of the source image (in pixels) passed into FaceDetector for performance considerations.
        /// Images larger that this size will be downscaled proportionally.
        /// </summary>
        /// <remarks>
        /// This is an arbitrary value that was chosen for this scenario, in which FaceDetector performance isn't too important but face
        /// detection accuracy is; a generous size is used.
        /// Your application may have different performance and accuracy needs and you'll need to decide how best to control input.
        /// </remarks>
        private readonly uint sourceImageHeightLimit = 1280;

        /// <summary>
        /// Reference back to the "root" page of the app.
        /// </summary>
        private MainPage rootPage;

        /// <summary>
        /// Initializes a new instance of the <see cref="Scenario1_DetectInPhoto" /> class.
        /// </summary>
        public Scenario1_DetectInPhoto()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Responds when we navigate to this page.
        /// </summary>
        /// <param name="e">Event data</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            this.rootPage = MainPage.Current;
        }

        /// <summary>
        /// Takes the photo image and FaceDetector results and assembles the visualization onto the Canvas.
        /// </summary>
        /// <param name="displaySource">Bitmap object holding the image we're going to display</param>
        /// <param name="foundFaces">List of detected faces; output from FaceDetector</param>
        private void SetupVisualization(WriteableBitmap displaySource, IList<DetectedFace> foundFaces)
        {
            this.PhotoCanvas.Background = new ImageBrush() { ImageSource = displaySource, Stretch = Stretch.Fill };
            MainPage.HighlightFaces(displaySource, foundFaces, this.PhotoCanvas, this.HighlightedFaceBox);
        }

        /// <summary>
        /// Clears the display of image and face boxes.
        /// </summary>
        private void ClearVisualization()
        {
            this.PhotoCanvas.Background = null;
            this.PhotoCanvas.Children.Clear();
            this.rootPage.NotifyUser(string.Empty, NotifyType.StatusMessage);
        }

        /// <summary>
        /// Computes a BitmapTransform to downscale the source image if it's too large. 
        /// </summary>
        /// <remarks>
        /// Performance of the FaceDetector degrades significantly with large images, and in most cases it's best to downscale
        /// the source bitmaps if they're too large before passing them into FaceDetector. Remember through, your application's performance needs will vary.
        /// </remarks>
        /// <param name="sourceDecoder">Source image decoder</param>
        /// <returns>A BitmapTransform object holding scaling values if source image is too large</returns>
        private BitmapTransform ComputeScalingTransformForSourceImage(BitmapDecoder sourceDecoder)
        {
            BitmapTransform transform = new BitmapTransform();

            if (sourceDecoder.PixelHeight > this.sourceImageHeightLimit)
            {
                float scalingFactor = (float)this.sourceImageHeightLimit / (float)sourceDecoder.PixelHeight;

                transform.ScaledWidth = (uint)Math.Floor(sourceDecoder.PixelWidth * scalingFactor);
                transform.ScaledHeight = (uint)Math.Floor(sourceDecoder.PixelHeight * scalingFactor);
            }

            return transform;
        }

        /// <summary>
        /// Loads an image file (selected by the user) and runs the FaceDetector on the loaded bitmap. If successful calls SetupVisualization to display the results.
        /// </summary>
        /// <param name="sender">Button user clicked</param>
        /// <param name="e">Event data</param>
        private async void OpenFile_Click(object sender, RoutedEventArgs e)
        {
            FileOpenPicker photoPicker = new FileOpenPicker();
            photoPicker.ViewMode = PickerViewMode.Thumbnail;
            photoPicker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            photoPicker.FileTypeFilter.Add(".jpg");
            photoPicker.FileTypeFilter.Add(".jpeg");
            photoPicker.FileTypeFilter.Add(".png");
            photoPicker.FileTypeFilter.Add(".bmp");

            StorageFile photoFile = await photoPicker.PickSingleFileAsync();
            if (photoFile == null)
            {
                return;
            }

            this.ClearVisualization();
            this.rootPage.NotifyUser("Opening...", NotifyType.StatusMessage);

            try
            {
                // Open the image file and decode the bitmap into memory.
                // We'll need to make 2 bitmap copies: one for the FaceDetector and another to display.
                using (IRandomAccessStream fileStream = await photoFile.OpenAsync(Windows.Storage.FileAccessMode.Read))
                {
                    BitmapDecoder decoder = await BitmapDecoder.CreateAsync(fileStream);
                    BitmapTransform transform = this.ComputeScalingTransformForSourceImage(decoder);
                    using (SoftwareBitmap originalBitmap = await decoder.GetSoftwareBitmapAsync(decoder.BitmapPixelFormat, BitmapAlphaMode.Ignore, transform, ExifOrientationMode.IgnoreExifOrientation, ColorManagementMode.DoNotColorManage))
                    {
                        // We need to convert the image into a format that's compatible with FaceDetector.
                        // Gray8 should be a good type but verify it against FaceDetector’s supported formats.
                        const BitmapPixelFormat InputPixelFormat = BitmapPixelFormat.Gray8;
                        if (FaceDetector.IsBitmapPixelFormatSupported(InputPixelFormat))
                        {
                            using (SoftwareBitmap detectorInput = SoftwareBitmap.Convert(originalBitmap, InputPixelFormat))
                            {
                                // Create a WritableBitmap for our visualization display; copy the original bitmap pixels to WriteableBitmap's buffer.
                                WriteableBitmap displaySource = new WriteableBitmap(originalBitmap.PixelWidth, originalBitmap.PixelHeight);
                                originalBitmap.CopyToBuffer(displaySource.PixelBuffer);

                                this.rootPage.NotifyUser("Detecting...", NotifyType.StatusMessage);

                                // Initialize our FaceDetector and execute it against our input image.
                                // NOTE: FaceDetector initialization can take a long time, and in most cases
                                // you should create a member variable and reuse the object.
                                // However, for simplicity in this scenario we instantiate a new instance each time.
                                FaceDetector detector = await FaceDetector.CreateAsync();
                                IList<DetectedFace> faces = await detector.DetectFacesAsync(detectorInput);

                                // Create our display using the available image and face results.
                                this.SetupVisualization(displaySource, faces);
                            }
                        }
                        else
                        {
                            this.rootPage.NotifyUser("PixelFormat '" + InputPixelFormat.ToString() + "' is not supported by FaceDetector", NotifyType.ErrorMessage);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                this.ClearVisualization();
                this.rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Updates any existing face bounding boxes in response to changes in the size of the Canvas.
        /// </summary>
        /// <param name="sender">Canvas object whose size has changed</param>
        /// <param name="e">Event data</param>
        private void PhotoCanvas_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            // If the Canvas is resized we must recompute a new scaling factor and
            // apply it to each face box.
            ImageBrush brush = (ImageBrush)this.PhotoCanvas.Background;
            if (brush != null)
            {
                MainPage.RepositionFaces((WriteableBitmap)brush.ImageSource, this.PhotoCanvas);
            }
        }
    }
}
