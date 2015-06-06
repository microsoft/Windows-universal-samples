// Copyright (c) Microsoft. All rights reserved.

#pragma once
#include "Scenario5Photos.g.h"
#include "PrintHelper.h"
#include <concrt.h>

namespace SDKTemplate
{
    /// <summary>
    /// Photo size options
    /// </summary>
    enum PhotoSize : byte
    {
        SizeFullPage,
        Size4x6,
        Size5x7,
        Size8x10
    };

    /// <summary>
    /// Photo scaling options
    /// </summary>
    enum Scaling : byte
    {
        ShrinkToFit,
        Crop
    };

    /// <summary>
    /// Printable page description
    /// </summary>
    public value class PageDescription
    {
    public:
        Windows::Foundation::Size Margin;
        Windows::Foundation::Size PageSize;
        Windows::Foundation::Size ViewablePageSize;
        Windows::Foundation::Size PictureViewSize;
        bool IsContentCropped;

        friend bool operator==(PageDescription pageDescriptiopn1, PageDescription pageDescription2)
        {
            // Detect if PageSize changed
            bool equal = (std::abs(pageDescriptiopn1.PageSize.Width - pageDescription2.PageSize.Width) < FLT_EPSILON) &&
                (std::abs(pageDescriptiopn1.PageSize.Height - pageDescription2.PageSize.Height) < FLT_EPSILON);

            // Detect if ViewablePageSize changed
            if (equal)
            {
                equal = (std::abs(pageDescriptiopn1.ViewablePageSize.Width - pageDescription2.ViewablePageSize.Width) < FLT_EPSILON) &&
                    (std::abs(pageDescriptiopn1.ViewablePageSize.Height - pageDescription2.ViewablePageSize.Height) < FLT_EPSILON);
            }

            // Detect if PictureViewSize changed
            if (equal)
            {
                equal = (std::abs(pageDescriptiopn1.PictureViewSize.Width - pageDescription2.PictureViewSize.Width) < FLT_EPSILON) &&
                    (std::abs(pageDescriptiopn1.PictureViewSize.Height - pageDescription2.PictureViewSize.Height) < FLT_EPSILON);
            }

            // Detect if cropping changed
            if (equal)
            {
                equal = pageDescriptiopn1.IsContentCropped == pageDescription2.IsContentCropped;
            }

            return equal;
        }

        friend bool operator!=(PageDescription pageDescriptiopn1, PageDescription pageDescription2)
        {
            return !(pageDescriptiopn1 == pageDescription2);
        }
    };

    ref class PhotosPrintHelper sealed : public PrintHelper
    {
    public:
        PhotosPrintHelper(Page^ scenarioPage) : PrintHelper(scenarioPage) { }

    protected:
        /// <summary>
        /// Option change event handler
        /// </summary>
        /// <param name="sender">The print task option details for which an option changed.</param>
        /// <param name="args">The event arguments containing the id of the option changed.</param>
        void PrintDetailedOptionsOptionChanged(Windows::Graphics::Printing::OptionDetails::PrintTaskOptionDetails^ sender,
            Windows::Graphics::Printing::OptionDetails::PrintTaskOptionChangedEventArgs^ args);

        /// <summary>
        /// This is the event handler for PrintManager.PrintTaskRequested.
        /// In order to ensure a good user experience, the system requires that the app handle the PrintTaskRequested event within the time specified
        /// by PrintTaskRequestedEventArgs->Request->Deadline.
        /// Therefore, we use this handler to only create the print task.
        /// The print settings customization can be done when the print document source is requested.
        /// </summary>
        /// <param name="sender">The print manager for which a print task request was made.</param>
        /// <param name="e">The print taks request associated arguments.</param>
        virtual void PrintTaskRequested(Windows::Graphics::Printing::PrintManager^ sender, Windows::Graphics::Printing::PrintTaskRequestedEventArgs^ e) override;

        /// <summary>
        /// Helper function used to triger a preview refresh
        /// </summary>
        void RefreshPreview();

        /// <summary>
        /// This is the event handler for Pagination.
        /// </summary>
        /// <param name="sender">The document for which pagination occurs.</param>
        /// <param name="e">The pagination event arguments containing the print options.</param>
        virtual void CreatePrintPreviewPages(Object^ sender, PaginateEventArgs^ e) override;

        /// <summary>
        /// This is the event handler for PrintDocument.GetPrintPreviewPage. It provides a specific print preview page,
        /// in the form of an UIElement, to an instance of PrintDocument.
        /// PrintDocument subsequently converts the UIElement into a page that the Windows print system can deal with.
        /// </summary>
        /// <param name="sender">The print document.</param>
        /// <param name="e">Arguments containing the preview requested page.</param>
        virtual void GetPrintPreviewPage(Object^ sender, GetPreviewPageEventArgs^ e) override;

        /// <summary>
        /// This is the event handler for PrintDocument.AddPages. It provides all pages to be printed, in the form of
        /// UIElements, to an instance of PrintDocument. PrintDocument subsequently converts the UIElements
        /// into a pages that the Windows print system can deal with.
        /// </summary>
        /// <param name="sender">The print document.</param>
        /// <param name="e">Add page event arguments containing a print task options reference</param>
        virtual void AddPrintPages(Object^ sender, AddPagesEventArgs^ e) override;

        /// <summary>
        /// Helper function that clears the page collection and also the pages attached to the "visual root".
        /// </summary>
        void ClearPageCollection();

    private:
        /// <summary>
        /// The app's number of photos.
        /// </summary>
        static const int NumberOfPhotos = 9;

        /// <summary>
        /// Constant for 96 DPI
        /// </summary>
        static const int DPI96 = 96;

        /// <summary>
        /// A map of UIElements used to store the print preview pages.
        /// </summary>
        std::map<int, UIElement^> m_pageCollection;

        /// <summary>
        /// Synchronization object used to secure access to the page collection and the visual root(printingRoot).
        /// </summary>
        concurrency::critical_section m_csPrintSync;

        /// <summary>
        /// Current size settings for the image.
        /// </summary>
        PhotoSize m_photoSize;

        /// <summary>
        /// Current scale settings for the image.
        /// </summary>
        Scaling m_photoScale;

        /// <summary>
        /// The current printer's page description used to create the content (size, margins, printable area).
        /// </summary>
        PageDescription m_currentPageDescription;

        /// <summary>
        /// A request "number" used to describe a Paginate - GetPreviewPage session.
        /// It is used by GetPreviewPage to determine, before calling SetPreviewPage, if the page content is out of date.
        /// Flow:
        /// Paginate will increment the request count and all subsequent GetPreviewPage calls will store a local copy and verify it before calling SetPreviewPage.
        /// If another Paginate event is triggered while some GetPreviewPage workers are still executing asynchronously
        /// their results will be discarded(ignored) because their request number is expired (the photo page description changed).
        /// </summary>
        LONGLONG m_requestCount;

        /// <summary>
        /// Generic swap of 2 values
        /// </summary>
        /// <typeparam name="T">typename</typeparam>
        /// <param name="v1">Value 1</param>
        /// <param name="v2">Value 2</param>
        template<class T> static void Swap(T& v1, T& v2);

        /// <summary>
        /// Generates a page containing a photo.
        /// The image will be rotated if detected that there is a gain from that regarding size (try to maximize photo size).
        /// </summary>
        /// <param name="photoNumber">The photo number.</param>
        /// <param name="pageDescription">The description of the printer page.</param>
        /// <returns>A task that will produce the page.</returns>
        concurrency::task<UIElement^> GeneratePageAsync(int photoNumber, PageDescription pageDescription);

        /// <summary>
        /// Loads an image from an uri source and performs a rotation based on the print target aspect.
        /// </summary>
        /// <param name="source">The location of the image.</param>
        /// <param name="landscape">A flag that indicates if the target(printer page) is in landscape mode.</param>
        concurrency::task<Windows::UI::Xaml::Media::Imaging::WriteableBitmap^> LoadBitmapAsync(Windows::Foundation::Uri^ source, bool landscape);
    };

    /// <summary>
    /// Photo printing scenario
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario5Photos sealed
    {
    public:
        Scenario5Photos();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        PrintHelper^ printHelper;
        void OnPrintButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
