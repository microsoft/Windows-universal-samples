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

using SDKTemplate;
using System;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Foundation.Collections;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.Storage.AccessCache;
using Windows.Storage.FileProperties;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace SimpleImaging
{
    /// <summary>
    /// Code-behind for Scenario 2, which demonstrates image editing using BitmapDecoder and BitmapEncoder.
    /// </summary>
    public sealed partial class Scenario2 : Page
    {
        // Exception HResult constants. These constants are defined using values from winerror.h.
        // This file format does not support the requested operation; for example, metadata or thumbnails.
        const int WINCODEC_ERR_UNSUPPORTEDOPERATION = unchecked((int)0x88982F81);
        // This file format does not support the requested property/metadata query.
        const int WINCODEC_ERR_PROPERTYNOTSUPPORTED = unchecked((int)0x88982F41);
        // There is no codec or component that can handle the requested operation; for example, encoding.
        const int WINCODEC_ERR_COMPONENTNOTFOUND = unchecked((int)0x88982F50);

        StorageItemAccessList m_futureAccess = StorageApplicationPermissions.FutureAccessList;
        IPropertySet m_localSettings = Windows.Storage.ApplicationData.Current.LocalSettings.Values;
        RotateTransform m_transform = new RotateTransform();
        string m_fileToken;
        uint m_displayWidthNonScaled;
        uint m_displayHeightNonScaled;
        double m_scaleFactor;
        PhotoOrientation m_userRotation;
        PhotoOrientation m_exifOrientation;
        bool m_disableExifOrientation;
        private MainPage rootPage;

        public Scenario2()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            App.Current.Suspending += new SuspendingEventHandler(SaveDataToPersistedState);

            // Reset scenario state before starting.
            ResetSessionState();

            // Attempt to load the previously saved scenario state.
            if (m_localSettings.ContainsKey("scenario2FileToken"))
            {
                RestoreDataFromPersistedState();
            }
        }

        /// <summary>
        /// When the application is about to enter a suspended state, save the user edited properties text.
        /// This method does not use the SuspensionManager helper class (defined in SuspensionManager.cs).
        /// </summary>
        private void SaveDataToPersistedState(object sender, SuspendingEventArgs args)
        {
            // Only save state if we have valid data.
            if (m_fileToken != null)
            {
                // Requesting a deferral prevents the application from being immediately suspended.
                SuspendingDeferral deferral = args.SuspendingOperation.GetDeferral();

                // LocalSettings does not support overwriting existing items, so first clear the collection.
                ResetPersistedState();

                m_localSettings.Add("scenario2FileToken", m_fileToken);
                m_localSettings.Add("scenario2Width", m_displayWidthNonScaled);
                m_localSettings.Add("scenario2Height", m_displayHeightNonScaled);
                m_localSettings.Add("scenario2Scale", m_scaleFactor);
                m_localSettings.Add("scenario2UserRotation",
                    Helpers.ConvertToExifOrientationFlag(m_userRotation)
                    );

                m_localSettings.Add("scenario2ExifOrientation",
                    Helpers.ConvertToExifOrientationFlag(m_exifOrientation)
                    );

                m_localSettings.Add("scenario2DisableExif", m_disableExifOrientation);

                deferral.Complete();
            }
        }

        /// <summary>
        /// Reads the file token and image transform variables from the persisted state and 
        /// restores the UI.
        /// </summary>
        private async void RestoreDataFromPersistedState()
        {
            try
            {
                rootPage.NotifyUser("Loading image file from persisted state...", NotifyType.StatusMessage);

                m_fileToken = (string)m_localSettings["scenario2FileToken"];
                m_displayWidthNonScaled = (uint)m_localSettings["scenario2Width"];
                m_displayHeightNonScaled = (uint)m_localSettings["scenario2Height"];
                m_scaleFactor = (double)m_localSettings["scenario2Scale"];

                PhotoOrientation desiredOrientation =
                    Helpers.ConvertToPhotoOrientation((ushort)m_localSettings["scenario2UserRotation"]);

                m_exifOrientation =
                    Helpers.ConvertToPhotoOrientation((ushort)m_localSettings["scenario2ExifOrientation"]);

                m_disableExifOrientation = (bool)m_localSettings["scenario2DisableExif"];

                // Display the image in the UI.
                StorageFile file = await m_futureAccess.GetFileAsync(m_fileToken);
                BitmapImage src = new BitmapImage();

                using (IRandomAccessStream stream = await file.OpenAsync(FileAccessMode.Read))
                {
                    await src.SetSourceAsync(stream);
                }
                PreviewImage.Source = src;
                AutomationProperties.SetName(PreviewImage, file.Name);

                // Display the image dimensions and transformation state in the UI.
                ExifOrientationTextblock.Text = Helpers.GetOrientationString(m_exifOrientation);
                ScaleSlider.Value = m_scaleFactor * 100;
                UpdateImageDimensionsUI();

                // Restore the image tag's rotation transform.
                while (desiredOrientation != m_userRotation)
                {
                    RotateRight_Click(null, null);
                }

                RotateRightButton.IsEnabled = true;
                RotateLeftButton.IsEnabled = true;
                SaveButton.IsEnabled = true;
                CloseButton.IsEnabled = true;
                SaveAsButton.IsEnabled = true;
                ScaleSlider.IsEnabled = true;
                rootPage.NotifyUser("Loaded image file from persisted state: " + file.Name, NotifyType.StatusMessage);
            }
            catch (Exception err)
            {
                rootPage.NotifyUser("Error: " + err.Message, NotifyType.ErrorMessage);
                ResetSessionState();
                ResetPersistedState();
            }
        }

        /// <summary>
        /// Load an image from disk and display properties.
        /// Invoked when the user clicks on the Open button.
        /// </summary>
        private async void Open_Click(object sender, RoutedEventArgs e)
        {
            ResetPersistedState();
            ResetSessionState();

            FileOpenPicker picker = new FileOpenPicker();
            Helpers.FillDecoderExtensions(picker.FileTypeFilter);
            picker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;

            var file = await picker.PickSingleFileAsync();
            if (file != null)
            {
                await LoadFileAsync(file);
            }
        }

        /// <summary>
        /// Load an image from a file and display some basic imaging properties.
        /// </summary>
        /// <param name="file">The image to load.</param>
        private async Task LoadFileAsync(StorageFile file)
        {
            try
            {
                await DisplayImageFileAsync(file);
                rootPage.NotifyUser("Loaded file from picker: " + file.Name, NotifyType.StatusMessage);
            }
            catch (Exception err)
            {
                rootPage.NotifyUser("Error: " + err.Message, NotifyType.ErrorMessage);
                ResetSessionState();
                ResetPersistedState();
            }
        }

        /// <summary>
        /// This function is called when the user clicks the Open button,
        /// and when they save (to refresh the image state).
        /// </summary>
        private async Task DisplayImageFileAsync(StorageFile file)
        {
            // Request persisted access permissions to the file the user selected.
            // This allows the app to directly load the file in the future without relying on a
            // broker such as the file picker.
            m_fileToken = m_futureAccess.Add(file);

            // Display the image in the UI.
            BitmapImage src = new BitmapImage();
            using (IRandomAccessStream stream = await file.OpenAsync(FileAccessMode.Read))
            {
                await src.SetSourceAsync(stream);
            }

            PreviewImage.Source = src;
            AutomationProperties.SetName(PreviewImage, file.Name);

            // Use BitmapDecoder to attempt to read EXIF orientation and image dimensions.
            await GetImageInformationAsync(file);

            ExifOrientationTextblock.Text = Helpers.GetOrientationString(m_exifOrientation);
            UpdateImageDimensionsUI();

            ScaleSlider.IsEnabled = true;
            RotateLeftButton.IsEnabled = true;
            RotateRightButton.IsEnabled = true;
            SaveButton.IsEnabled = true;
            SaveAsButton.IsEnabled = true;
            CloseButton.IsEnabled = true;
        }

        /// <summary>
        /// Asynchronously attempts to get the oriented dimensions and EXIF orientation from the image file.
        /// Sets member variables instead of returning a value with the Task.
        /// </summary>
        /// <param name="file"></param>
        /// <returns></returns>
        private async Task GetImageInformationAsync(StorageFile file)
        {
            using (IRandomAccessStream stream = await file.OpenAsync(FileAccessMode.Read))
            {
                BitmapDecoder decoder = await BitmapDecoder.CreateAsync(stream);

                // The orientedPixelWidth and Height members provide the image dimensions
                // reflecting any EXIF orientation.
                m_displayHeightNonScaled = decoder.OrientedPixelHeight;
                m_displayWidthNonScaled = decoder.OrientedPixelWidth;

                try
                {
                    // Property access using BitmapProperties is similar to using
                    // Windows.Storage.FileProperties (see Scenario 1); BitmapProperties accepts
                    // property keys such as "System.Photo.Orientation".
                    // The EXIF orientation flag can be also be read using native metadata queries
                    // such as "/app1/ifd/{ushort=274}" (JPEG) or "/ifd/{ushort=274}" (TIFF).
                    string[] requestedProperties = { "System.Photo.Orientation" };
                    BitmapPropertySet retrievedProperties =
                        await decoder.BitmapProperties.GetPropertiesAsync(requestedProperties);

                    // Check to see if the property exists in the file.
                    if (retrievedProperties.ContainsKey("System.Photo.Orientation"))
                    {
                        // EXIF orientation ("System.Photo.Orientation") is stored as a 16-bit unsigned integer.
                        m_exifOrientation = Helpers.ConvertToPhotoOrientation(
                            (ushort)retrievedProperties["System.Photo.Orientation"].Value
                            );
                    }
                }
                catch (Exception err)
                {
                    switch (err.HResult)
                    {
                        // If the file format does not support properties continue without applying EXIF orientation.
                        case WINCODEC_ERR_UNSUPPORTEDOPERATION:
                        case WINCODEC_ERR_PROPERTYNOTSUPPORTED:
                            m_disableExifOrientation = true;
                            break;
                        default:
                            throw;
                    }
                }
            }
        }

        /// <summary>
        /// When the user clicks Rotate Right, rotate the ImageViewbox by 90 degrees clockwise,
        /// and update the dimensions.
        /// </summary>
        private void RotateRight_Click(object sender, RoutedEventArgs e)
        {
            m_userRotation = Helpers.Add90DegreesCW(m_userRotation);

            // Swap width and height.
            uint temp = m_displayHeightNonScaled;
            m_displayHeightNonScaled = m_displayWidthNonScaled;
            m_displayWidthNonScaled = temp;

            UpdateImageDimensionsUI();
            UpdateImageRotation(m_userRotation);
        }

        /// <summary>
        /// When the user clicks Rotate Left, rotate the ImageViewbox by 90 degrees counterclockwise,
        /// and update the dimensions.
        /// </summary>
        private void RotateLeft_Click(object sender, RoutedEventArgs e)
        {
            m_userRotation = Helpers.Add90DegreesCCW(m_userRotation);

            // Swap width and height.
            uint temp = m_displayHeightNonScaled;
            m_displayHeightNonScaled = m_displayWidthNonScaled;
            m_displayWidthNonScaled = temp;

            UpdateImageDimensionsUI();
            UpdateImageRotation(m_userRotation);
        }

        /// <summary>
        /// Applies the user-provided scale and rotation operation to the original image file.
        /// The "Transcoding" mode is used which preserves image metadata and performs other
        /// optimizations when possible.
        /// 
        /// This method attempts to perform "soft" rotation using the EXIF orientation flag when possible,
        /// but falls back to a hard rotation of the image pixels.
        /// </summary>
        private async void Save_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                rootPage.NotifyUser("Saving file...", NotifyType.StatusMessage);

                // Create a new encoder and initialize it with data from the original file.
                // The encoder writes to an in-memory stream, we then copy the contents to the file.
                // This allows the application to perform in-place editing of the file: any unedited data
                // is copied to the destination, and the original file is overwritten
                // with updated data.
                StorageFile file = await m_futureAccess.GetFileAsync(m_fileToken);
                using (IRandomAccessStream fileStream = await file.OpenAsync(FileAccessMode.ReadWrite),
                                           memStream = new InMemoryRandomAccessStream())
                {
                    BitmapDecoder decoder = await BitmapDecoder.CreateAsync(fileStream);

                    // Use the native (no orientation applied) image dimensions because we want to handle
                    // orientation ourselves.
                    uint originalWidth = decoder.PixelWidth;
                    uint originalHeight = decoder.PixelHeight;

                    // Set the encoder's destination to the temporary, in-memory stream.
                    BitmapEncoder encoder = await BitmapEncoder.CreateForTranscodingAsync(memStream, decoder);

                    // Scaling occurs before flip/rotation, therefore use the original dimensions
                    // (no orientation applied) as parameters for scaling.
                    // Dimensions are rounded down by BitmapEncoder to the nearest integer.
                    if (m_scaleFactor != 1.0)
                    {
                        encoder.BitmapTransform.ScaledWidth = (uint)(originalWidth * m_scaleFactor);
                        encoder.BitmapTransform.ScaledHeight = (uint)(originalHeight * m_scaleFactor);

                        // Fant is a relatively high quality interpolation mode.
                        encoder.BitmapTransform.InterpolationMode = BitmapInterpolationMode.Fant;
                    }

                    // If the file format supports EXIF orientation ("System.Photo.Orientation") then
                    // update the orientation flag to reflect any user-specified rotation.
                    // Otherwise, perform a hard rotate using BitmapTransform.
                    if (m_disableExifOrientation == false)
                    {
                        BitmapPropertySet properties = new BitmapPropertySet();
                        ushort netExifOrientation = Helpers.ConvertToExifOrientationFlag(
                            Helpers.AddPhotoOrientation(m_userRotation, m_exifOrientation)
                            );

                        // BitmapProperties requires the application to explicitly declare the type
                        // of the property to be written - this is different from FileProperties which
                        // automatically coerces the value to the correct type. System.Photo.Orientation
                        // is defined as an unsigned 16 bit integer.
                        BitmapTypedValue orientationTypedValue = new BitmapTypedValue(
                            netExifOrientation,
                            Windows.Foundation.PropertyType.UInt16
                            );

                        properties.Add("System.Photo.Orientation", orientationTypedValue);
                        await encoder.BitmapProperties.SetPropertiesAsync(properties);
                    }
                    else
                    {
                        encoder.BitmapTransform.Rotation = Helpers.ConvertToBitmapRotation(m_userRotation);
                    }

                    // Attempt to generate a new thumbnail to reflect any rotation operation.
                    encoder.IsThumbnailGenerated = true;

                    try
                    {
                        await encoder.FlushAsync();
                    }
                    catch (Exception err)
                    {
                        switch (err.HResult)
                        {
                            case WINCODEC_ERR_UNSUPPORTEDOPERATION:
                                // If the encoder does not support writing a thumbnail, then try again
                                // but disable thumbnail generation.
                                encoder.IsThumbnailGenerated = false;
                                break;
                            default:
                                throw;
                        }
                    }

                    if (encoder.IsThumbnailGenerated == false)
                    {
                        await encoder.FlushAsync();
                    }

                    // Now that the file has been written to the temporary stream, copy the data to the file.
                    memStream.Seek(0);
                    fileStream.Seek(0);
                    fileStream.Size = 0;
                    await RandomAccessStream.CopyAsync(memStream, fileStream);
                }

                // Because the original file has been overwritten, reload it in the UI.
                ResetSessionState();
                await DisplayImageFileAsync(file);

                rootPage.NotifyUser("Successfully saved file: " + file.Name, NotifyType.StatusMessage);
            }
            catch (Exception err)
            {
                if (err.HResult == WINCODEC_ERR_COMPONENTNOTFOUND)
                {
                    // Some image formats (e.g. ICO) do not have encoders.
                    rootPage.NotifyUser("Error: this file format may not support editing.", NotifyType.ErrorMessage);
                }
                else
                {
                    rootPage.NotifyUser("Error: " + err.Message, NotifyType.ErrorMessage);
                }

                ResetPersistedState();
                ResetSessionState();
            }
        }

        /// <summary>
        /// Applies the user-provided scale and rotation operation to a new image file picked by the user.
        /// This method writes the edited pixel data to the new file without
        /// any regard to existing metadata or other information in the original file.
        /// </summary>
        private async void SaveAs_Click(object sender, RoutedEventArgs e)
        {
            FileSavePicker picker = new FileSavePicker();
            picker.FileTypeChoices.Add("JPEG image", new string[] { ".jpg" });
            picker.FileTypeChoices.Add("PNG image", new string[] { ".png" });
            picker.FileTypeChoices.Add("BMP image", new string[] { ".bmp" });
            picker.DefaultFileExtension = ".png";
            picker.SuggestedFileName = "Output file";
            picker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;

            var file = await picker.PickSaveFileAsync();
            if (file != null)
            {
                await LoadSaveFileAsync(file);
            }
        }

        private async Task LoadSaveFileAsync(StorageFile outputFile)
        {
            try
            {
                StorageFile inputFile = await m_futureAccess.GetFileAsync(m_fileToken);
                Guid encoderId;

                switch (outputFile.FileType)
                {
                    case ".png":
                        encoderId = BitmapEncoder.PngEncoderId;
                        break;
                    case ".bmp":
                        encoderId = BitmapEncoder.BmpEncoderId;
                        break;
                    case ".jpg":
                    default:
                        encoderId = BitmapEncoder.JpegEncoderId;
                        break;
                }

                using (IRandomAccessStream inputStream = await inputFile.OpenAsync(FileAccessMode.Read),
                           outputStream = await outputFile.OpenAsync(FileAccessMode.ReadWrite))
                {
                    // BitmapEncoder expects an empty output stream; the user may have selected a
                    // pre-existing file.
                    outputStream.Size = 0;

                    // Get pixel data from the decoder. We apply the user-requested transforms on the
                    // decoded pixels to take advantage of potential optimizations in the decoder.
                    BitmapDecoder decoder = await BitmapDecoder.CreateAsync(inputStream);
                    BitmapTransform transform = new BitmapTransform();

                    // Scaling occurs before flip/rotation, therefore use the original dimensions
                    // (no orientation applied) as parameters for scaling.
                    transform.ScaledHeight = (uint)(decoder.PixelHeight * m_scaleFactor);
                    transform.ScaledWidth = (uint)(decoder.PixelWidth * m_scaleFactor);
                    transform.Rotation = Helpers.ConvertToBitmapRotation(m_userRotation);

                    // Fant is a relatively high quality interpolation mode.
                    transform.InterpolationMode = BitmapInterpolationMode.Fant;

                    // The BitmapDecoder indicates what pixel format and alpha mode best match the
                    // natively stored image data. This can provide a performance and/or quality gain.
                    BitmapPixelFormat format = decoder.BitmapPixelFormat;
                    BitmapAlphaMode alpha = decoder.BitmapAlphaMode;

                    PixelDataProvider pixelProvider = await decoder.GetPixelDataAsync(
                        format,
                        alpha,
                        transform,
                        ExifOrientationMode.RespectExifOrientation,
                        ColorManagementMode.ColorManageToSRgb
                        );

                    byte[] pixels = pixelProvider.DetachPixelData();

                    // Write the pixel data onto the encoder. Note that we can't simply use the
                    // BitmapTransform.ScaledWidth and ScaledHeight members as the user may have
                    // requested a rotation (which is applied after scaling).
                    BitmapEncoder encoder = await BitmapEncoder.CreateAsync(encoderId, outputStream);
                    encoder.SetPixelData(
                        format,
                        alpha,
                        (uint)((double)m_displayWidthNonScaled * m_scaleFactor),
                        (uint)((double)m_displayHeightNonScaled * m_scaleFactor),
                        decoder.DpiX,
                        decoder.DpiY,
                        pixels
                        );

                    await encoder.FlushAsync();

                    rootPage.NotifyUser("Successfully saved a copy: " + outputFile.Name, NotifyType.StatusMessage);
                }
            }
            catch (Exception err)
            {
                rootPage.NotifyUser("Error: " + err.Message, NotifyType.ErrorMessage);
                ResetPersistedState();
                ResetSessionState();
            }
        }

        /// <summary>
        /// Handles when the slider controlling image scale is changed.
        /// </summary>
        private void ScaleSlider_ValueChanged(object sender, Windows.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            // Ignore any value changed events the control is set up and we are accepting user input.
            if ((ScaleSlider != null) && (ScaleSlider.IsEnabled == true))
            {
                // The slider control uses percentages (0 to 100) while m_scaleFactor is a coefficient (0 to 1).
                m_scaleFactor = e.NewValue / 100;
                UpdateImageDimensionsUI();
            }
        }

        /// <summary>
        /// Display the image dimensions and transformation state in the UI.
        /// </summary>
        private void UpdateImageDimensionsUI()
        {
            ScaleTextblock.Text = (m_scaleFactor * 100).ToString() + "%";
            WidthTextblock.Text = Math.Floor(((double)m_displayWidthNonScaled * m_scaleFactor)).ToString() + " pixels";
            HeightTextblock.Text = Math.Floor(((double)m_displayHeightNonScaled * m_scaleFactor)).ToString() + " pixels";
            UserRotationTextblock.Text = Helpers.GetOrientationString(m_userRotation);
        }

        /// <summary>
        /// Sets the RotationTransform angle of the Image (technically, its parent Viewbox).
        /// </summary>
        private void UpdateImageRotation(PhotoOrientation rotation)
        {
            switch (rotation)
            {
                case PhotoOrientation.Rotate270:
                    // Note that the PhotoOrientation enumeration uses a counterclockwise convention,
                    // while the RotationTransform uses a clockwise convention.
                    m_transform.Angle = 90;
                    break;
                case PhotoOrientation.Rotate180:
                    m_transform.Angle = 180;
                    break;
                case PhotoOrientation.Rotate90:
                    m_transform.Angle = 270;
                    break;
                case PhotoOrientation.Normal:
                default:
                    m_transform.Angle = 0;
                    break;
            }
        }

        /// <summary>
        /// Closing the file brings the scenario back to the default initialized state.
        /// </summary>
        private void Close_Click(object sender, RoutedEventArgs e)
        {
            ResetSessionState();
            ResetPersistedState();
            rootPage.NotifyUser("Closed file", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Clears all of the state for this scenario that is stored in app data.
        /// </summary>
        private void ResetPersistedState()
        {
            m_localSettings.Remove("scenario2FileToken");
            m_localSettings.Remove("scenario2Scale");
            m_localSettings.Remove("scenario2Rotation");
            m_localSettings.Remove("scenario2Width");
            m_localSettings.Remove("scenario2Height");
            m_localSettings.Remove("scenario2UserRotation");
            m_localSettings.Remove("scenario2ExifOrientation");
            m_localSettings.Remove("scenario2DisableExif");
        }

        /// <summary>
        /// Clears all of the state that is stored in memory and in the UI.
        /// </summary>
        private void ResetSessionState()
        {
            m_fileToken = null;
            m_displayHeightNonScaled = 0;
            m_displayWidthNonScaled = 0;
            m_scaleFactor = 1;
            m_userRotation = PhotoOrientation.Normal;
            m_exifOrientation = PhotoOrientation.Normal;
            m_disableExifOrientation = false;

            RotateLeftButton.IsEnabled = false;
            RotateRightButton.IsEnabled = false;
            SaveButton.IsEnabled = false;
            SaveAsButton.IsEnabled = false;
            CloseButton.IsEnabled = false;

            PreviewImage.Source = null;
            m_transform.CenterX = ImageViewbox.Width / 2;
            m_transform.CenterY = ImageViewbox.Height / 2;
            ImageViewbox.RenderTransform = m_transform;
            UpdateImageRotation(PhotoOrientation.Normal);

            ScaleTextblock.Text = "";
            ScaleSlider.Value = 100;
            ScaleSlider.IsEnabled = false;
            HeightTextblock.Text = "";
            WidthTextblock.Text = "";
            UserRotationTextblock.Text = "";
            ExifOrientationTextblock.Text = "";
        }
    }
}
