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
using System.Threading;
using System.Threading.Tasks;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Printing;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Media;
using Windows.Graphics.Printing;
using Windows.Graphics.Printing.OptionDetails;
using Windows.Graphics.Imaging;
using Windows.Storage;
using SDKTemplate;

namespace PrintSample
{
    /// <summary>
    /// Photo size options
    /// </summary>
    public enum PhotoSize : byte
    {
        SizeFullPage,
        Size4x6,
        Size5x7,
        Size8x10
    }

    /// <summary>
    /// Photo scaling options
    /// </summary>
    public enum Scaling : byte
    {
        ShrinkToFit,
        Crop
    }

    /// <summary>
    /// Printable page description
    /// </summary>
    public class PageDescription : IEquatable<PageDescription>
    {
        public Size Margin;
        public Size PageSize;
        public Size ViewablePageSize;
        public Size PictureViewSize;
        public bool IsContentCropped;

        public bool Equals(PageDescription other)
        {
            // Detect if PageSize changed
            bool equal = (Math.Abs(PageSize.Width - other.PageSize.Width) < double.Epsilon) &&
                (Math.Abs(PageSize.Height - other.PageSize.Height) < double.Epsilon);

            // Detect if ViewablePageSize changed
            if (equal)
            {
                equal = (Math.Abs(ViewablePageSize.Width - other.ViewablePageSize.Width) < double.Epsilon) &&
                    (Math.Abs(ViewablePageSize.Height - other.ViewablePageSize.Height) < double.Epsilon);
            }

            // Detect if PictureViewSize changed
            if (equal)
            {
                equal = (Math.Abs(PictureViewSize.Width - other.PictureViewSize.Width) < double.Epsilon) &&
                    (Math.Abs(PictureViewSize.Height - other.PictureViewSize.Height) < double.Epsilon);
            }

            // Detect if cropping changed
            if (equal)
            {
                equal = IsContentCropped == other.IsContentCropped;
            }

            return equal;
        }
    }

    class PhotosPrintHelper : PrintHelper
    {
        #region Scenario specific constants.
        /// <summary>
        /// The app's number of photos
        /// </summary>
        private const int NumberOfPhotos = 6;

        /// <summary>
        /// Constant for 96 DPI
        /// </summary>
        private const int DPI96 = 96;

        #endregion

        /// <summary>
        /// Current size settings for the image
        /// </summary>
        private PhotoSize photoSize;

        /// <summary>
        /// Current scale settings for the image
        /// </summary>
        private Scaling photoScale;

        /// <summary>
        /// A map of UIElements used to store the print preview pages.
        /// </summary>
        private Dictionary<int, UIElement> pageCollection = new Dictionary<int, UIElement>();

        /// <summary>
        /// Synchronization object used to sync access to pageCollection and the visual root(PrintingRoot).
        /// </summary>
        private static object printSync = new object();

        /// <summary>
        /// The current printer's page description used to create the content (size, margins, printable area)
        /// </summary>
        private PageDescription currentPageDescription;

        /// <summary>
        /// A request "number" used to describe a Paginate - GetPreviewPage session.
        /// It is used by GetPreviewPage to determine, before calling SetPreviewPage, if the page content is out of date.
        /// Flow:
        /// Paginate will increment the request count and all subsequent GetPreviewPage calls will store a local copy and verify it before calling SetPreviewPage.
        /// If another Paginate event is triggered while some GetPreviewPage workers are still executing asynchronously
        /// their results will be discarded(ignored) because their request number is expired (the photo page description changed).
        /// </summary>
        private long requestCount;

        public PhotosPrintHelper(Page scenarioPage) : base(scenarioPage)
        {
            photoSize = PhotoSize.SizeFullPage;
            photoScale = Scaling.ShrinkToFit;
        }

        /// <summary>
        /// This is the event handler for PrintManager.PrintTaskRequested.
        /// In order to ensure a good user experience, the system requires that the app handle the PrintTaskRequested event within the time specified
        /// by PrintTaskRequestedEventArgs->Request->Deadline.
        /// Therefore, we use this handler to only create the print task.
        /// The print settings customization can be done when the print document source is requested.
        /// </summary>
        /// <param name="sender">The print manager for which a print task request was made.</param>
        /// <param name="e">The print taks request associated arguments.</param>
        protected override void PrintTaskRequested(Windows.Graphics.Printing.PrintManager sender, Windows.Graphics.Printing.PrintTaskRequestedEventArgs e)
        {
            PrintTask printTask = null;
            printTask = e.Request.CreatePrintTask("C# Printing SDK Sample", sourceRequestedArgs =>
            {
                PrintTaskOptionDetails printDetailedOptions = PrintTaskOptionDetails.GetFromPrintTaskOptions(printTask.Options);

                // Choose the printer options to be shown.
                // The order in which the options are appended determines the order in which they appear in the UI
                printDetailedOptions.DisplayedOptions.Clear();
                printDetailedOptions.DisplayedOptions.Add(Windows.Graphics.Printing.StandardPrintTaskOptions.MediaSize);
                printDetailedOptions.DisplayedOptions.Add(Windows.Graphics.Printing.StandardPrintTaskOptions.Copies);

                // Create a new list option.
                PrintCustomItemListOptionDetails photoSize = printDetailedOptions.CreateItemListOption("photoSize", "Photo Size");
                photoSize.AddItem("SizeFullPage", "Full Page");
                photoSize.AddItem("Size4x6", "4 x 6 in");
                photoSize.AddItem("Size5x7", "5 x 7 in");
                photoSize.AddItem("Size8x10", "8 x 10 in");

                // Add the custom option to the option list.
                printDetailedOptions.DisplayedOptions.Add("photoSize");

                PrintCustomItemListOptionDetails scaling = printDetailedOptions.CreateItemListOption("scaling", "Scaling");
                scaling.AddItem("ShrinkToFit", "Shrink To Fit");
                scaling.AddItem("Crop", "Crop");

                // Add the custom option to the option list.
                printDetailedOptions.DisplayedOptions.Add("scaling");

                // Set default orientation to landscape.
                printTask.Options.Orientation = PrintOrientation.Landscape;

                // Register for print task option changed notifications.
                printDetailedOptions.OptionChanged += PrintDetailedOptionsOptionChanged;

                // Register for print task Completed notification.
                // Print Task event handler is invoked when the print job is completed.
                printTask.Completed += async (s, args) =>
                {
                    await scenarioPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                    {
                        ClearPageCollection();

                        // Reset image options to default values.
                        this.photoScale = Scaling.ShrinkToFit;
                        this.photoSize = PhotoSize.SizeFullPage;

                        // Reset the current page description
                        currentPageDescription = null;

                        // Notify the user when the print operation fails.
                        if (args.Completion == PrintTaskCompletion.Failed)
                        {
                            MainPage.Current.NotifyUser("Failed to print.", NotifyType.ErrorMessage);
                        }
                    });
                };

                // Set the document source.
                sourceRequestedArgs.SetSource(printDocumentSource);
            });
        }

        /// <summary>
        /// Option change event handler
        /// </summary>
        /// <param name="sender">The print task option details for which an option changed.</param>
        /// <param name="args">The event arguments containing the id of the changed option.</param>
        private async void PrintDetailedOptionsOptionChanged(PrintTaskOptionDetails sender, PrintTaskOptionChangedEventArgs args)
        {
            bool invalidatePreview = false;

            // For this scenario we are interested only when the 2 custom options change (photoSize & scaling) in order to trigger a preview refresh.
            // Default options that change page aspect will trigger preview invalidation (refresh) automatically.
            // It is safe to ignore verifying other options and(or) combinations here because during Paginate event(CreatePrintPreviewPages) we check if the PageDescription changed.
            if (args.OptionId == null)
            {
                return;
            }

            string optionId = args.OptionId.ToString();

            if (optionId == "photoSize")
            {
                IPrintOptionDetails photoSizeOption = sender.Options[optionId];
                string photoSizeValue = photoSizeOption.Value as string;

                if (!string.IsNullOrEmpty(photoSizeValue))
                {
                    switch (photoSizeValue)
                    {
                        case "SizeFullPage":
                            photoSize = PhotoSize.SizeFullPage;
                            break;
                        case "Size4x6":
                            photoSize = PhotoSize.Size4x6;
                            break;
                        case "Size5x7":
                            photoSize = PhotoSize.Size5x7;
                            break;
                        case "Size8x10":
                            photoSize = PhotoSize.Size8x10;
                            break;
                    }
                    invalidatePreview = true;
                }
            }

            if (optionId == "scaling")
            {
                IPrintOptionDetails scalingOption = sender.Options[optionId];
                string scalingValue = scalingOption.Value as string;

                if (!string.IsNullOrEmpty(scalingValue))
                {
                    switch (scalingValue)
                    {
                        case "Crop":
                            photoScale = Scaling.Crop;
                            break;
                        case "ShrinkToFit":
                            photoScale = Scaling.ShrinkToFit;
                            break;
                    }
                    invalidatePreview = true;
                }
            }

            // Invalidate preview if one of the 2 options (photoSize, scaling) changed.
            if (invalidatePreview)
            {
                await scenarioPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, printDocument.InvalidatePreview);
            }
        }

        /// <summary>
        /// This is the event handler for Pagination.
        /// </summary>
        /// <param name="sender">The document for which pagination occurs.</param>
        /// <param name="e">The pagination event arguments containing the print options.</param>
        protected override void CreatePrintPreviewPages(object sender, Windows.UI.Xaml.Printing.PaginateEventArgs e)
        {

            PrintDocument printDoc = (PrintDocument)sender;

            // A new "session" starts with each paginate event.
            Interlocked.Increment(ref requestCount);

            PageDescription pageDescription = new PageDescription();

            // Get printer's page description.
            PrintTaskOptionDetails printDetailedOptions = PrintTaskOptionDetails.GetFromPrintTaskOptions(e.PrintTaskOptions);
            PrintPageDescription printPageDescription = e.PrintTaskOptions.GetPageDescription(0);

            // Reset the error state
            printDetailedOptions.Options["photoSize"].ErrorText = string.Empty;

            // Compute the printing page description (page size & center printable area)
            pageDescription.PageSize = printPageDescription.PageSize;

            pageDescription.Margin.Width = Math.Max(
                printPageDescription.ImageableRect.Left,
                printPageDescription.ImageableRect.Right - printPageDescription.PageSize.Width);

            pageDescription.Margin.Height = Math.Max(
                printPageDescription.ImageableRect.Top,
                printPageDescription.ImageableRect.Bottom - printPageDescription.PageSize.Height);

            pageDescription.ViewablePageSize.Width = printPageDescription.PageSize.Width - pageDescription.Margin.Width * 2;
            pageDescription.ViewablePageSize.Height = printPageDescription.PageSize.Height - pageDescription.Margin.Height * 2;

            // Compute print photo area.
            switch (photoSize)
            {
                case PhotoSize.Size4x6:
                    pageDescription.PictureViewSize.Width = 4 * DPI96;
                    pageDescription.PictureViewSize.Height = 6 * DPI96;
                    break;
                case PhotoSize.Size5x7:
                    pageDescription.PictureViewSize.Width = 5 * DPI96;
                    pageDescription.PictureViewSize.Height = 7 * DPI96;
                    break;
                case PhotoSize.Size8x10:
                    pageDescription.PictureViewSize.Width = 8 * DPI96;
                    pageDescription.PictureViewSize.Height = 10 * DPI96;
                    break;
                case PhotoSize.SizeFullPage:
                    pageDescription.PictureViewSize.Width = pageDescription.ViewablePageSize.Width;
                    pageDescription.PictureViewSize.Height = pageDescription.ViewablePageSize.Height;
                    break;
            }

            // Try to maximize photo-size based on it's aspect-ratio
            if ((pageDescription.ViewablePageSize.Width > pageDescription.ViewablePageSize.Height) && (photoSize != PhotoSize.SizeFullPage))
            {
                var swap = pageDescription.PictureViewSize.Width;
                pageDescription.PictureViewSize.Width = pageDescription.PictureViewSize.Height;
                pageDescription.PictureViewSize.Height = swap;
            }

            pageDescription.IsContentCropped = photoScale == Scaling.Crop;

            // Recreate content only when :
            // - there is no current page description
            // - the current page description doesn't match the new one
            if (currentPageDescription == null || !currentPageDescription.Equals(pageDescription))
            {
                ClearPageCollection();

                if (pageDescription.PictureViewSize.Width > pageDescription.ViewablePageSize.Width ||
                    pageDescription.PictureViewSize.Height > pageDescription.ViewablePageSize.Height)
                {
                    printDetailedOptions.Options["photoSize"].ErrorText = "Photo doesn’t fit on the selected paper";

                    // Inform preview that it has only 1 page to show.
                    printDoc.SetPreviewPageCount(1, PreviewPageCountType.Intermediate);

                    // Add a custom "preview" unavailable page
                    lock (printSync)
                    {
                        pageCollection[0] = new PreviewUnavailable(pageDescription.PageSize, pageDescription.ViewablePageSize);
                    }
                }
                else
                {
                    // Inform preview that is has #NumberOfPhotos pages to show.
                    printDoc.SetPreviewPageCount(NumberOfPhotos, PreviewPageCountType.Intermediate);
                }

                currentPageDescription = pageDescription;
            }
        }

        /// <summary>
        /// This is the event handler for PrintDocument.GetPrintPreviewPage. It provides a specific print page preview,
        /// in the form of an UIElement, to an instance of PrintDocument.
        /// PrintDocument subsequently converts the UIElement into a page that the Windows print system can deal with.
        /// </summary>
        /// <param name="sender">The print documet.</param>
        /// <param name="e">Arguments containing the requested page preview.</param>
        protected async override void GetPrintPreviewPage(object sender, GetPreviewPageEventArgs e)
        {
            // Store a local copy of the request count to use later to determine if the computed page is out of date.
            // If the page preview is unavailable an async operation will generate the content.
            // When the operation completes there is a chance that a pagination request was already made therefore making this page obsolete.
            // If the page is obsolete throw away the result (don't call SetPreviewPage) since a new GetPrintPreviewPage will server that request.
            long requestNumber = 0;
            Interlocked.Exchange(ref requestNumber, requestCount);
            int pageNumber = e.PageNumber;

            UIElement page;
            bool pageReady = false;

            // Try to get the page if it was previously generated.
            lock (printSync)
            {
                pageReady = pageCollection.TryGetValue(pageNumber - 1, out page);
            }

            if (!pageReady)
            {
                // The page is not available yet.
                page = await GeneratePageAsync(pageNumber, currentPageDescription);

                // If the ticket changed discard the result since the content is outdated.
                if (Interlocked.CompareExchange(ref requestNumber, requestNumber, requestCount) != requestCount)
                {
                    return;
                }

                // Store the page in the list in case an invalidate happens but the content doesn't need to be regenerated.

                lock (printSync)
                {
                    pageCollection[pageNumber - 1] = page;

                    // Add the newly created page to the printing root which is part of the visual tree and force it to go
                    // through layout so that the linked containers correctly distribute the content inside them.
                    PrintCanvas.Children.Add(page);
                    PrintCanvas.InvalidateMeasure();
                    PrintCanvas.UpdateLayout();
                }
            }

            PrintDocument printDoc = (PrintDocument)sender;

            // Send the page to preview.
            printDoc.SetPreviewPage(pageNumber, page);
        }

        /// <summary>
        /// This is the event handler for PrintDocument.AddPages. It provides all pages to be printed, in the form of
        /// UIElements, to an instance of PrintDocument. PrintDocument subsequently converts the UIElements
        /// into a pages that the Windows print system can deal with.
        /// </summary>
        /// <param name="sender">The print document.</param>
        /// <param name="e">Arguments containing the print task options.</param>
        protected override async void AddPrintPages(object sender, AddPagesEventArgs e)
        {
            PrintDocument printDoc = (PrintDocument)sender;

            // Loop over all of the preview pages
            for (int i = 0; i < NumberOfPhotos; i++)
            {
                UIElement page = null;
                bool pageReady = false;

                lock (printSync)
                {
                    pageReady = pageCollection.TryGetValue(i, out page);
                }

                if (!pageReady)
                {
                    // If the page is not ready create a task that will generate its content.
                    page = await GeneratePageAsync(i + 1, currentPageDescription);
                }

                printDoc.AddPage(page);
            }

            // Indicate that all of the print pages have been provided.
            printDoc.AddPagesComplete();

            // Reset the current page description as soon as possible since the PrintTask.Completed event might fire later (long running job)
            currentPageDescription = null;
        }

        /// <summary>
        /// Helper function that clears the page collection and also the pages attached to the "visual root".
        /// </summary>
        private void ClearPageCollection()
        {
            lock (printSync)
            {
                pageCollection.Clear();
                PrintCanvas.Children.Clear();
            }
        }

        /// <summary>
        /// Generic swap of 2 values
        /// </summary>
        /// <typeparam name="T">typename</typeparam>
        /// <param name="v1">Value 1</param>
        /// <param name="v2">Value 2</param>
        private static void Swap<T>(ref T v1, ref T v2)
        {
            T swap = v1;
            v1 = v2;
            v2 = swap;
        }

        /// <summary>
        /// Generates a page containing a photo.
        /// The image will be rotated if detected that there is a gain from that regarding size (try to maximize photo size).
        /// </summary>
        /// <param name="photoNumber">The photo number.</param>
        /// <param name="pageDescription">The description of the printer page.</param>
        /// <returns>A task that will return the page.</returns>
        private async Task<UIElement> GeneratePageAsync(int photoNumber, PageDescription pageDescription)
        {
            Canvas page = new Canvas
            {
                Width = pageDescription.PageSize.Width,
                Height = pageDescription.PageSize.Height
            };

            Canvas viewablePage = new Canvas()
            {
                Width = pageDescription.ViewablePageSize.Width,
                Height = pageDescription.ViewablePageSize.Height
            };

            viewablePage.SetValue(Canvas.LeftProperty, pageDescription.Margin.Width);
            viewablePage.SetValue(Canvas.TopProperty, pageDescription.Margin.Height);

            // The image "frame" which also acts as a viewport
            Grid photoView = new Grid
            {
                Width = pageDescription.PictureViewSize.Width,
                Height = pageDescription.PictureViewSize.Height
            };

            // Center the frame.
            photoView.SetValue(Canvas.LeftProperty, (viewablePage.Width - photoView.Width) / 2);
            photoView.SetValue(Canvas.TopProperty, (viewablePage.Height - photoView.Height) / 2);

            // Return an async task that will complete when the image is fully loaded.
            WriteableBitmap bitmap = await LoadBitmapAsync(
                new Uri(string.Format("ms-appx:///Assets/photo{0}.jpg", photoNumber)),
                pageDescription.PageSize.Width > pageDescription.PageSize.Height);

            if (bitmap != null)
            {
                Image image = new Image
                {
                    Source = bitmap,
                    HorizontalAlignment = Windows.UI.Xaml.HorizontalAlignment.Center,
                    VerticalAlignment = Windows.UI.Xaml.VerticalAlignment.Center
                };

                // Use the real image size when croping or if the image is smaller then the target area (prevent a scale-up).
                if (photoScale == Scaling.Crop ||
                    (bitmap.PixelWidth <= pageDescription.PictureViewSize.Width &&
                    bitmap.PixelHeight <= pageDescription.PictureViewSize.Height))
                {
                    image.Stretch = Stretch.None;
                    image.Width = bitmap.PixelWidth;
                    image.Height = bitmap.PixelHeight;
                }

                // Add the newly created image to the visual root.
                photoView.Children.Add(image);
                viewablePage.Children.Add(photoView);
                page.Children.Add(viewablePage);
            }

            // Return the page with the image centered.
            return page;
        }

        /// <summary>
        /// Loads an image from an uri source and performs a rotation based on the print target aspect.
        /// </summary>
        /// <param name="source">The location of the image.</param>
        /// <param name="landscape">A flag that indicates if the target (printer page) is in landscape mode.</param>
        /// <returns>A task that will return the loaded bitmap.</returns>
        private async Task<WriteableBitmap> LoadBitmapAsync(Uri source, bool landscape)
        {
            var file = await StorageFile.GetFileFromApplicationUriAsync(source);
            using (var stream = await file.OpenAsync(FileAccessMode.Read))
            {
                BitmapDecoder decoder = await BitmapDecoder.CreateAsync(stream);

                BitmapTransform transform = new BitmapTransform();
                transform.Rotation = BitmapRotation.None;
                uint width = decoder.PixelWidth;
                uint height = decoder.PixelHeight;

                if (landscape && width < height)
                {
                    transform.Rotation = BitmapRotation.Clockwise270Degrees;
                    Swap(ref width, ref height);
                }
                else if (!landscape && width > height)
                {
                    transform.Rotation = BitmapRotation.Clockwise90Degrees;
                    Swap(ref width, ref height);
                }

                PixelDataProvider pixelData = await decoder.GetPixelDataAsync(
                    BitmapPixelFormat.Bgra8,    // WriteableBitmap uses BGRA format.
                    BitmapAlphaMode.Straight,
                    transform,
                    ExifOrientationMode.IgnoreExifOrientation,    // This sample ignores Exif orientation.
                    ColorManagementMode.DoNotColorManage);

                WriteableBitmap bitmap = new WriteableBitmap((int)width, (int)height);
                var pixelBuffer = pixelData.DetachPixelData();
                using (var pixelStream = bitmap.PixelBuffer.AsStream())
                {
                    pixelStream.Write(pixelBuffer, 0, (int)pixelStream.Length);
                }

                return bitmap;
            }
        }
    }

    /// <summary>
    /// Photo printing scenario
    /// </summary>
    public sealed partial class Scenario5Photos : Page
    {
        private PhotosPrintHelper printHelper;

        public Scenario5Photos()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// This is the click handler for the 'Print' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void OnPrintButtonClick(object sender, RoutedEventArgs e)
        {
            await printHelper.ShowPrintUIAsync();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (PrintManager.IsSupported())
            {
                // Tell the user how to print
                MainPage.Current.NotifyUser("Print contract registered with customization, use the Print button to print.", NotifyType.StatusMessage);
            }
            else
            {
                // Remove the print button
                InvokePrintingButton.Visibility = Visibility.Collapsed;

                // Inform user that Printing is not supported
                MainPage.Current.NotifyUser("Printing is not supported.", NotifyType.ErrorMessage);

                // Printing-related event handlers will never be called if printing
                // is not supported, but it's okay to register for them anyway.
            }

            // Initalize common helper class and register for printing
            printHelper = new PhotosPrintHelper(this);
            printHelper.RegisterForPrinting();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (printHelper != null)
            {
                printHelper.UnregisterForPrinting();
            }
        }
    }
}
