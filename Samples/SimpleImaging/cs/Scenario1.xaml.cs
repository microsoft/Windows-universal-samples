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
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.Storage.AccessCache;
using Windows.Storage.FileProperties;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace SimpleImaging
{
    /// <summary>
    /// Code-behind for Scenario 1, which demonstrates image properties with FileProperties.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        // Exception HResult constants. These constants are defined using values from winerror.h.
        // Either the requested value to set on the Windows property is not valid, or the Windows property
        // is not supported by the file format.
        const int E_INVALIDARG = unchecked((int)0x80070057);

        StorageItemAccessList m_futureAccess = StorageApplicationPermissions.FutureAccessList;
        IPropertySet m_localSettings = Windows.Storage.ApplicationData.Current.LocalSettings.Values;
        ImageProperties m_imageProperties;
        string m_fileToken;
        private MainPage rootPage;

        public Scenario1()
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
            if (m_localSettings.ContainsKey("scenario1FileToken"))
            {
                RestoreDataFromPersistedState();
            }
        }

        /// <summary>
        /// Invoked when the user clicks on the Open button.
        /// </summary>
        private async void Open_Click(object sender, RoutedEventArgs e)
        {
            ResetSessionState();
            ResetPersistedState();

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
        /// When the application is about to enter a suspended state, save the user edited properties text.
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

                m_localSettings.Add("scenario1Title", TitleTextbox.Text);
                m_localSettings.Add("scenario1Keywords", KeywordsTextbox.Text);
                m_localSettings.Add("scenario1DateTaken", DateTakenTextblock.Text);
                m_localSettings.Add("scenario1Make", MakeTextblock.Text);
                m_localSettings.Add("scenario1Model", ModelTextblock.Text);
                m_localSettings.Add("scenario1Orientation", OrientationTextblock.Text);
                m_localSettings.Add("scenario1LatDeg", LatDegTextbox.Text);
                m_localSettings.Add("scenario1LatMin", LatMinTextbox.Text);
                m_localSettings.Add("scenario1LatSec", LatSecTextbox.Text);
                m_localSettings.Add("scenario1LatRef", LatRefTextbox.Text);
                m_localSettings.Add("scenario1LongDeg", LongDegTextbox.Text);
                m_localSettings.Add("scenario1LongMin", LongMinTextbox.Text);
                m_localSettings.Add("scenario1LongSec", LongSecTextbox.Text);
                m_localSettings.Add("scenario1LongRef", LongRefTextbox.Text);
                m_localSettings.Add("scenario1Exposure", ExposureTextblock.Text);
                m_localSettings.Add("scenario1FNumber", FNumberTextblock.Text);
                m_localSettings.Add("scenario1FileToken", m_fileToken);

                deferral.Complete();
            }
        }

        /// <summary>
        /// Reads the file token and property text from the persisted state, and calls
        /// DisplayImageUIAsync().
        /// </summary>
        private async void RestoreDataFromPersistedState()
        {
            try
            {
                m_fileToken = m_localSettings["scenario1FileToken"].ToString();
                StorageFile file = await m_futureAccess.GetFileAsync(m_fileToken);
                m_imageProperties = await file.Properties.GetImagePropertiesAsync();

                Dictionary<string, string> propertyText = new Dictionary<string, string>();
                propertyText.Add("Title", m_localSettings["scenario1Title"].ToString());
                propertyText.Add("Keywords", m_localSettings["scenario1Keywords"].ToString());
                propertyText.Add("DateTaken", m_localSettings["scenario1DateTaken"].ToString());
                propertyText.Add("Make", m_localSettings["scenario1Make"].ToString());
                propertyText.Add("Model", m_localSettings["scenario1Model"].ToString());
                propertyText.Add("Orientation", m_localSettings["scenario1Orientation"].ToString());
                propertyText.Add("LatDeg", m_localSettings["scenario1LatDeg"].ToString());
                propertyText.Add("LatMin", m_localSettings["scenario1LatMin"].ToString());
                propertyText.Add("LatSec", m_localSettings["scenario1LatSec"].ToString());
                propertyText.Add("LatRef", m_localSettings["scenario1LatRef"].ToString());
                propertyText.Add("LongDeg", m_localSettings["scenario1LongDeg"].ToString());
                propertyText.Add("LongMin", m_localSettings["scenario1LongMin"].ToString());
                propertyText.Add("LongSec", m_localSettings["scenario1LongSec"].ToString());
                propertyText.Add("LongRef", m_localSettings["scenario1LongRef"].ToString());
                propertyText.Add("Exposure", m_localSettings["scenario1Exposure"].ToString());
                propertyText.Add("FNumber", m_localSettings["scenario1FNumber"].ToString());

                await DisplayImageUIAsync(file, propertyText);

                rootPage.NotifyUser(
                    "Loaded file from persisted state: " + file.Name,
                    NotifyType.StatusMessage
                    );

                CloseButton.IsEnabled = true;
                ApplyButton.IsEnabled = true;
            }
            catch (Exception err)
            {
                rootPage.NotifyUser("Error: " + err.Message, NotifyType.ErrorMessage);
                ResetSessionState();
                ResetPersistedState();
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
                // Request persisted access permissions to the file the user selected.
                // This allows the app to directly load the file in the future without relying on a
                // broker such as the file picker.
                m_fileToken = m_futureAccess.Add(file);

                // Windows.Storage.FileProperties.ImageProperties provides convenience access to
                // commonly-used properties such as geolocation and keywords. It also accepts
                // queries for Windows property system keys such as "System.Photo.Aperture".
                m_imageProperties = await file.Properties.GetImagePropertiesAsync();

                string[] requests =
                {
                    "System.Photo.ExposureTime",        // In seconds
                    "System.Photo.FNumber"              // F-stop values defined by EXIF spec
                };

                IDictionary<string, object> retrievedProps =
                    await m_imageProperties.RetrievePropertiesAsync(requests);
                await DisplayImageUIAsync(file, GetImagePropertiesForDisplay(retrievedProps));

                rootPage.NotifyUser(
                    "Loaded file from picker: " + file.Name,
                    NotifyType.StatusMessage
                    );

                CloseButton.IsEnabled = true;
                ApplyButton.IsEnabled = true;
            }
            catch (Exception err)
            {
                rootPage.NotifyUser("Error: " + err.Message, NotifyType.ErrorMessage);
                ResetPersistedState();
                ResetSessionState();
            }
        }

        /// <summary>
        /// Gathers the imaging properties read from a file and formats them into a single object
        /// that can be consumed by DisplayImageUIAsync(). This method also reads from m_imageProperties.
        /// </summary>
        /// <param name="retrievedProps">Contains System.Photo.ExposureTime and System.Photo.FNumber</param>
        /// <returns>Dictionary of strings to be used in DisplayImageUI().</returns>
        private IDictionary<string, string> GetImagePropertiesForDisplay(IDictionary<string, object> retrievedProps)
        {
            var propertyText = new Dictionary<string, string>();

            // Some of the properties need to be converted/formatted.
            string keywordsText = String.Join(Environment.NewLine, m_imageProperties.Keywords);

            string exposureText;
            if (retrievedProps.ContainsKey("System.Photo.ExposureTime"))
            {
                exposureText = ((double)retrievedProps["System.Photo.ExposureTime"] * 1000).ToString() + " ms";
            }
            else
            {
                exposureText = "";
            }

            string fNumberText;
            if (retrievedProps.ContainsKey("System.Photo.FNumber"))
            {
                fNumberText = ((double)retrievedProps["System.Photo.FNumber"]).ToString("F1");
            }
            else
            {
                fNumberText = "";
            }

            string orientationText = Helpers.GetOrientationString(m_imageProperties.Orientation);

            string latRefText, longRefText, latDegText, latMinText, latSecText, longDegText, longMinText, longSecText;

            // Do a simple check if GPS data exists.
            if ((m_imageProperties.Latitude.HasValue) && (m_imageProperties.Longitude.HasValue))
            {
                double latitude = m_imageProperties.Latitude.Value;
                double longitude = m_imageProperties.Longitude.Value;

                // Latitude and longitude are returned as double precision numbers,
                // but we want to convert to degrees/minutes/seconds format.
                latRefText = (latitude >= 0) ? "N" : "S";
                longRefText = (longitude >= 0) ? "E" : "W";
                double latDeg = Math.Floor(Math.Abs(latitude));
                latDegText = latDeg.ToString();
                double latMin = Math.Floor((Math.Abs(latitude) - latDeg) * 60);
                latMinText = latMin.ToString();
                latSecText = ((Math.Abs(latitude) - latDeg - latMin / 60) * 3600).ToString();
                double longDeg = Math.Floor(Math.Abs(longitude));
                longDegText = longDeg.ToString();
                double longMin = Math.Floor((Math.Abs(longitude) - longDeg) * 60);
                longMinText = longMin.ToString();
                longSecText = ((Math.Abs(longitude) - longDeg - longMin / 60) * 3600).ToString();
            }
            else
            {
                latRefText = longRefText = latDegText = latMinText = latSecText = longDegText = longMinText = longSecText = "";
            }

            propertyText.Add("Title", m_imageProperties.Title);
            propertyText.Add("Keywords", keywordsText);
            propertyText.Add("DateTaken", m_imageProperties.DateTaken.ToString());
            propertyText.Add("Make", m_imageProperties.CameraManufacturer);
            propertyText.Add("Model", m_imageProperties.CameraModel);
            propertyText.Add("Orientation", orientationText);
            propertyText.Add("LatRef", latRefText);
            propertyText.Add("LongRef", longRefText);
            propertyText.Add("LatDeg", latDegText);
            propertyText.Add("LatMin", latMinText);
            propertyText.Add("LatSec", latSecText);
            propertyText.Add("LongDeg", longDegText);
            propertyText.Add("LongMin", longMinText);
            propertyText.Add("LongSec", longSecText);
            propertyText.Add("Exposure", exposureText);
            propertyText.Add("FNumber", fNumberText);

            return propertyText;
        }

        /// <summary>
        /// Asynchronously displays the image file and properties text in the UI. This method is called
        /// when the user loads a new file, or when the app resumes from a suspended state.
        /// </summary>
        /// <param name="file">The image to be displayed.</param>
        /// <param name="propertyText">Collection of property text to be displayed. Must contain the following keys:
        /// Title, Keywords, DateTaken, Make, Model, LatRef, LongRef, LatDeg, LatMin, LatSec, LongDeg, LongMin,
        /// LongSec, Orientation, Exposure, FNumber.</param>
        private async Task DisplayImageUIAsync(StorageFile file, IDictionary<string, string> propertyText)
        {
            BitmapImage src = new BitmapImage();
            using (IRandomAccessStream stream = await file.OpenAsync(FileAccessMode.Read))
            {
                await src.SetSourceAsync(stream);
            }

            PreviewImage.Source = src;
            AutomationProperties.SetName(PreviewImage, file.Name);

            TitleTextbox.Text = propertyText["Title"];
            KeywordsTextbox.Text = propertyText["Keywords"];
            DateTakenTextblock.Text = propertyText["DateTaken"];
            MakeTextblock.Text = propertyText["Make"];
            ModelTextblock.Text = propertyText["Model"];
            OrientationTextblock.Text = propertyText["Orientation"];
            LatDegTextbox.Text = propertyText["LatDeg"];
            LatMinTextbox.Text = propertyText["LatMin"];
            LatRefTextbox.Text = propertyText["LatRef"];
            LongDegTextbox.Text = propertyText["LongDeg"];
            LongMinTextbox.Text = propertyText["LongMin"];
            LongRefTextbox.Text = propertyText["LongRef"];
            ExposureTextblock.Text = propertyText["Exposure"];
            FNumberTextblock.Text = propertyText["FNumber"];

            // Truncate the latitude and longitude seconds data to 4 decimal places (precision of 1/10000th).
            if (propertyText["LatSec"].Length > 7)
            {
                LatSecTextbox.Text = propertyText["LatSec"].Substring(0, 7);
            }
            else
            {
                LatSecTextbox.Text = propertyText["LatSec"];
            }

            if (propertyText["LongSec"].Length > 7)
            {
                LongSecTextbox.Text = propertyText["LongSec"].Substring(0, 7);
            }
            else
            {
                LongSecTextbox.Text = propertyText["LongSec"];
            }
        }

        /// <summary>
        /// Updates the file with the user-edited properties. The m_imageProperties object
        /// is still usable after the method completes.
        /// </summary>
        private async void Apply_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Saving file...", NotifyType.StatusMessage);

            m_imageProperties.Title = TitleTextbox.Text;

            // Keywords are stored as an IList of strings.
            if (KeywordsTextbox.Text.Length > 0)
            {
                string[] keywordsArray = KeywordsTextbox.Text.Split('\n');
                m_imageProperties.Keywords.Clear();

                for (uint i = 0; i < keywordsArray.Length; i++)
                {
                    m_imageProperties.Keywords.Add(keywordsArray[i]);
                }
            }

            PropertySet propertiesToSave = new PropertySet();

            // Perform some simple validation of the GPS data and package it in a format
            // better suited for writing to the file.
            bool gpsWriteFailed = false;
            double[] latitude = new double[3];
            double[] longitude = new double[3];
            string latitudeRef = String.Empty;
            string longitudeRef = String.Empty;

            try
            {
                latitude[0] = Math.Floor(Double.Parse(LatDegTextbox.Text));
                latitude[1] = Math.Floor(Double.Parse(LatMinTextbox.Text));
                latitude[2] = Double.Parse(LatSecTextbox.Text);

                longitude[0] = Math.Floor(Double.Parse(LongDegTextbox.Text));
                longitude[1] = Math.Floor(Double.Parse(LongMinTextbox.Text));
                longitude[2] = Double.Parse(LongSecTextbox.Text);

                latitudeRef = LatRefTextbox.Text.ToUpper();
                longitudeRef = LongRefTextbox.Text.ToUpper();
            }
            catch (Exception) // Treat any exception as invalid GPS data.
            {
                gpsWriteFailed = true;
            }

            if ((latitude[0] >= 0 && latitude[0] <= 90) &&
                (latitude[1] >= 0 && latitude[1] <= 60) &&
                (latitude[2] >= 0 && latitude[2] <= 60) &&
                (latitudeRef == "N" || latitudeRef == "S") &&
                (longitude[0] >= 0 && longitude[0] <= 180) &&
                (latitude[1] >= 0 && longitude[1] <= 60) &&
                (longitude[2] >= 0 && longitude[2] <= 60) &&
                (longitudeRef == "E" || longitudeRef == "W"))
            {
                propertiesToSave.Add("System.GPS.LatitudeRef", latitudeRef);
                propertiesToSave.Add("System.GPS.LongitudeRef", longitudeRef);

                // The Latitude and Longitude properties are read-only. Instead,
                // write to System.GPS.LatitudeNumerator, LatitudeDenominator, etc.
                // These are length 3 arrays of integers. For simplicity, the
                // seconds data is rounded to the nearest 10000th.
                uint[] latitudeNumerator =
                {
                    (uint) latitude[0],
                    (uint) latitude[1],
                    (uint) (latitude[2] * 10000)
                };

                uint[] longitudeNumerator =
                {
                    (uint) longitude[0],
                    (uint) longitude[1],
                    (uint) (longitude[2] * 10000)
                };

                // LatitudeDenominator and LongitudeDenominator share the same values.
                uint[] denominator =
                {
                    1,
                    1,
                    10000
                };

                propertiesToSave.Add("System.GPS.LatitudeNumerator", latitudeNumerator);
                propertiesToSave.Add("System.GPS.LatitudeDenominator", denominator);
                propertiesToSave.Add("System.GPS.LongitudeNumerator", longitudeNumerator);
                propertiesToSave.Add("System.GPS.LongitudeDenominator", denominator);
            }
            else
            {
                gpsWriteFailed = true;
            }

            try
            {
                // SavePropertiesAsync commits edits to the top level properties (e.g. Title) as
                // well as any Windows properties contained within the propertiesToSave parameter.
                await m_imageProperties.SavePropertiesAsync(propertiesToSave);

                rootPage.NotifyUser(gpsWriteFailed ? "GPS data invalid; other properties successfully updated" :
                        "All properties successfully updated", NotifyType.StatusMessage);
            }
            catch (Exception err)
            {
                switch (err.HResult)
                {
                    case E_INVALIDARG:
                        // Some imaging formats, such as PNG and BMP, do not support Windows properties.
                        // Other formats do not support all Windows properties.
                        // For example, JPEG does not support System.FlagStatus.
                        rootPage.NotifyUser("Error: A property value is invalid, or the file format does " +
                            "not support one or more requested properties.", NotifyType.ErrorMessage);

                        break;

                    default:
                        rootPage.NotifyUser("Error: " + err.Message, NotifyType.ErrorMessage);
                        break;
                }

                ResetSessionState();
                ResetPersistedState();
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
        /// This method does not use the SuspensionManager helper class (defined in SuspensionManager.cs).
        /// </summary>
        private void ResetPersistedState()
        {
            m_localSettings.Remove("scenario1FileToken");
            m_localSettings.Remove("scenario1Title");
            m_localSettings.Remove("scenario1Keywords");
            m_localSettings.Remove("scenario1DateTaken");
            m_localSettings.Remove("scenario1Make");
            m_localSettings.Remove("scenario1Model");
            m_localSettings.Remove("scenario1Orientation");
            m_localSettings.Remove("scenario1LatDeg");
            m_localSettings.Remove("scenario1LatMin");
            m_localSettings.Remove("scenario1LatSec");
            m_localSettings.Remove("scenario1LatRef");
            m_localSettings.Remove("scenario1LongDeg");
            m_localSettings.Remove("scenario1LongMin");
            m_localSettings.Remove("scenario1LongSec");
            m_localSettings.Remove("scenario1LongRef");
            m_localSettings.Remove("scenario1Exposure");
            m_localSettings.Remove("scenario1FNumber");
        }

        /// <summary>
        /// Clears all of the state that is stored in memory and in the UI.
        /// </summary>
        private void ResetSessionState()
        {
            m_imageProperties = null;
            m_fileToken = null;

            CloseButton.IsEnabled = false;
            ApplyButton.IsEnabled = false;
            PreviewImage.Source = null;

            TitleTextbox.Text = "";
            KeywordsTextbox.Text = "";
            DateTakenTextblock.Text = "";
            MakeTextblock.Text = "";
            ModelTextblock.Text = "";
            OrientationTextblock.Text = "";
            LatDegTextbox.Text = "";
            LatMinTextbox.Text = "";
            LatSecTextbox.Text = "";
            LatRefTextbox.Text = "";
            LongDegTextbox.Text = "";
            LongMinTextbox.Text = "";
            LongSecTextbox.Text = "";
            LongRefTextbox.Text = "";
            ExposureTextblock.Text = "";
            FNumberTextblock.Text = "";
        }
    }
}
