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
using Windows.Foundation;
using Windows.Globalization;
using Windows.Graphics.Imaging;
using Windows.Media.Ocr;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class OcrFileImage : Page
    {
        // A pointer back to the main page.
        // This is needed if you want to call methods in MainPage such as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        // Bitmap holder of currently loaded image.
        private SoftwareBitmap bitmap;

        // Recognized words overlay boxes.
        private List<WordOverlay> wordBoxes = new List<WordOverlay>();

        public OcrFileImage()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// Updates OCR available languages and loads sample image.
        /// </summary>
        /// <param name="e"></param>
        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            UpdateAvailableLanguages();

            await LoadSampleImage();
        }

        /// <summary>
        /// Updates list of ocr languages available on device.
        /// </summary>
        private void UpdateAvailableLanguages()
        {
            if (!UserLanguageToggle.IsOn)
            {
                // Check if any OCR language is available on device.
                if (OcrEngine.AvailableRecognizerLanguages.Count > 0)
                {
                    LanguageList.ItemsSource = OcrEngine.AvailableRecognizerLanguages;
                    LanguageList.SelectedIndex = 0;
                    LanguageList.IsEnabled = true;
                }
                else
                {
                    // Prevent OCR if no OCR languages are available on device.
                    UserLanguageToggle.IsEnabled = false;
                    LanguageList.IsEnabled = false;
                    ExtractButton.IsEnabled = false;

                    rootPage.NotifyUser("No available OCR languages.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                LanguageList.ItemsSource = null;
                LanguageList.IsEnabled = false;

                rootPage.NotifyUser(
                    "Run OCR in first OCR available language from UserProfile.GlobalizationPreferences.Languages list.",
                    NotifyType.StatusMessage);
            }
        }

        /// <summary>
        /// Loads image from file to bitmap and displays it in UI.
        /// </summary>
        /// <param name="file"></param>
        /// <returns></returns>
        private async Task LoadImage(StorageFile file)
        {
            using (var stream = await file.OpenAsync(Windows.Storage.FileAccessMode.Read))
            {
                var decoder = await BitmapDecoder.CreateAsync(stream);

                bitmap = await decoder.GetSoftwareBitmapAsync(BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);

                var imgSource = new WriteableBitmap(bitmap.PixelWidth, bitmap.PixelHeight);
                bitmap.CopyToBuffer(imgSource.PixelBuffer);
                PreviewImage.Source = imgSource;
            }
        }

        private async Task LoadSampleImage()
        {
            // Load sample "Hello World" image.
            var file = await Windows.ApplicationModel.Package.Current.InstalledLocation.GetFileAsync("Assets\\splash-sdk.png");
            await LoadImage(file);
        }

        /// <summary>
        /// Clears extracted text and text overlay from previous OCR.
        /// </summary>
        private void ClearResults()
        {
            TextOverlay.RenderTransform = null;
            ExtractedTextBox.Text = String.Empty;
            TextOverlay.Children.Clear();
            wordBoxes.Clear();
        }

        /// <summary>
        ///  Update word box transform to match current UI size.
        /// </summary>
        private void UpdateWordBoxTransform()
        {
            // Used for text overlay.
            // Prepare scale transform for words since image is not displayed in original size.
            ScaleTransform scaleTrasform = new ScaleTransform
            {
                CenterX = 0,
                CenterY = 0,
                ScaleX = PreviewImage.ActualWidth / bitmap.PixelWidth,
                ScaleY = PreviewImage.ActualHeight / bitmap.PixelHeight
            };

            foreach (var item in wordBoxes)
            {
                item.Transform(scaleTrasform);
            }
        }

        /// <summary>
        /// This is event handler for 'Extract' button.
        /// Recognizes text from image and displays it.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void ExtractButton_Tapped(object sender, Windows.UI.Xaml.Input.TappedRoutedEventArgs e)
        {
            ClearResults();

            // Check if OcrEngine supports image resoulution.
            if (bitmap.PixelWidth > OcrEngine.MaxImageDimension || bitmap.PixelHeight > OcrEngine.MaxImageDimension)
            {
                rootPage.NotifyUser(
                    String.Format("Bitmap dimensions ({0}x{1}) are too big for OCR.", bitmap.PixelWidth, bitmap.PixelHeight) +
                    "Max image dimension is " + OcrEngine.MaxImageDimension + ".",
                    NotifyType.ErrorMessage);

                return;
            }

            OcrEngine ocrEngine = null;

            if (UserLanguageToggle.IsOn)
            {
                // Try to create OcrEngine for first supported language from UserProfile.GlobalizationPreferences.Languages list.
                // If none of the languages are available on device, method returns null.
                ocrEngine = OcrEngine.TryCreateFromUserProfileLanguages();
            }
            else
            {
                // Try to create OcrEngine for specified language.
                // If language is not supported on device, method returns null.
                ocrEngine = OcrEngine.TryCreateFromLanguage(LanguageList.SelectedValue as Language);
            }

            if (ocrEngine != null)
            {
                // Recognize text from image.
                var ocrResult = await ocrEngine.RecognizeAsync(bitmap);

                // Display recognized text.
                ExtractedTextBox.Text = ocrResult.Text;

                if (ocrResult.TextAngle != null)
                {
                    // If text is detected under some angle in this sample scenario we want to
                    // overlay word boxes over original image, so we rotate overlay boxes.
                    TextOverlay.RenderTransform = new RotateTransform
                    {
                        Angle = (double)ocrResult.TextAngle,
                        CenterX = PreviewImage.ActualWidth / 2,
                        CenterY = PreviewImage.ActualHeight / 2
                    };
                }

                // Create overlay boxes over recognized words.
                foreach (var line in ocrResult.Lines)
                {
                    Rect lineRect = Rect.Empty;
                    foreach (var word in line.Words)
                    {
                        lineRect.Union(word.BoundingRect);
                    }

                    // Determine if line is horizontal or vertical.
                    // Vertical lines are supported only in Chinese Traditional and Japanese languages.
                    bool isVerticalLine = lineRect.Height > lineRect.Width;

                    foreach (var word in line.Words)
                    {
                        WordOverlay wordBoxOverlay = new WordOverlay(word);

                        // Keep references to word boxes.
                        wordBoxes.Add(wordBoxOverlay);

                        // Define overlay style.
                        var overlay = new Border()
                        {
                            Style = isVerticalLine ?
                                        (Style)this.Resources["HighlightedWordBoxVerticalLine"] :
                                        (Style)this.Resources["HighlightedWordBoxHorizontalLine"]
                        };

                        // Bind word boxes to UI.
                        overlay.SetBinding(Border.MarginProperty, wordBoxOverlay.CreateWordPositionBinding());
                        overlay.SetBinding(Border.WidthProperty, wordBoxOverlay.CreateWordWidthBinding());
                        overlay.SetBinding(Border.HeightProperty, wordBoxOverlay.CreateWordHeightBinding());

                        // Put the filled textblock in the results grid.
                        TextOverlay.Children.Add(overlay);
                    }
                }

                // Rescale word boxes to match current UI size.
                UpdateWordBoxTransform();

                rootPage.NotifyUser(
                    "Image is OCRed for " + ocrEngine.RecognizerLanguage.DisplayName + " language.",
                    NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("Selected language is not available.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Occures when user language toogle state is changed.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void UserLanguageToggle_Toggled(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            UpdateAvailableLanguages();
        }

        /// <summary>
        /// This is event handler for 'Sample' button.
        /// It loads image with 'Hello World' text and displays it in UI.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void SampleButton_Tapped(object sender, Windows.UI.Xaml.Input.TappedRoutedEventArgs e)
        {
            ClearResults();

            // Load sample "Hello World" image.
            await LoadSampleImage();

            rootPage.NotifyUser("Loaded sample image.", NotifyType.StatusMessage);
        }

        /// <summary>
        /// This is event handler for 'Load' button.
        /// It opens file picked and load selected image in UI..
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void LoadButton_Tapped(object sender, Windows.UI.Xaml.Input.TappedRoutedEventArgs e)
        {
            var picker = new FileOpenPicker()
            {
                SuggestedStartLocation = PickerLocationId.PicturesLibrary,
                FileTypeFilter = { ".jpg", ".jpeg", ".png" },
            };

            var file = await picker.PickSingleFileAsync();
            if (file != null)
            {
                ClearResults();

                await LoadImage(file);

                rootPage.NotifyUser(
                    String.Format("Loaded image from file: {0} ({1}x{2}).", file.Name, bitmap.PixelWidth, bitmap.PixelHeight),
                    NotifyType.StatusMessage);
            }
        }

        /// <summary>
        /// Occures when selected language is changed in available languages combo box.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void LanguageList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ClearResults();

            var lang = LanguageList.SelectedValue as Language;
            if (lang != null)
            {
                rootPage.NotifyUser(
                    "Selected OCR language is " + lang.DisplayName + ". " +
                        OcrEngine.AvailableRecognizerLanguages.Count + " OCR language(s) are available. " +
                        "Check combo box for full list.",
                    NotifyType.StatusMessage);
            }
        }

        /// <summary>
        /// Occurs when displayed image size changes.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void PreviewImage_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            // Update word overlay boxes.
            UpdateWordBoxTransform();

            // Update image rotation center.
            var rotate = TextOverlay.RenderTransform as RotateTransform;
            if (rotate != null)
            {
                rotate.CenterX = PreviewImage.ActualWidth / 2;
                rotate.CenterY = PreviewImage.ActualHeight / 2;
            }
        }
    }
}
