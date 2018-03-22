// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario4PageRange.g.h"
#include "PrintHelper.h"

namespace SDKTemplate
{
    class InvalidPageException
    {
    public:
        InvalidPageException(const std::wstring &message)
        {
            m_displayMessage = message;
        }

        std::wstring get_DisplayMessage()
        {
            return m_displayMessage;
        }

    private:
        std::wstring m_displayMessage;
    };

    ref class PageRangePrintHelper sealed : public PrintHelper
    {
    public:
        PageRangePrintHelper(Page^ scenarioPage);

    protected:
        /// <summary>
        /// This is the event handler for PrintManager.PrintTaskRequested.
        /// In order to ensure a good user experience, the system requires that the app handle the PrintTaskRequested event within the time specified by PrintTaskRequestedEventArgs->Request->Deadline.
        /// Therefore, we use this handler to only create the print task.
        /// The print settings customization can be done when the print document source is requested.
        /// </summary>
        /// <param name="sender">PrintManager</param>
        /// <param name="e">PrintTaskRequestedEventArgs</param>
        virtual void PrintTaskRequested(Windows::Graphics::Printing::PrintManager^ sender, Windows::Graphics::Printing::PrintTaskRequestedEventArgs^ e) override;

        /// <summary>
        /// This is called when the preview pages have been recreated. We use it to call ValidatePageRangeOption to make
        /// sure that the Page Ranges are still valid
        /// </summary>
        /// <param name="printTaskOptions"></param>
        virtual void OnPreviewPagesCreated(Windows::Graphics::Printing::PrintTaskOptions^ printTaskOptions) override;

        /// <summary>
        /// Deals with validating that the Page Ranges only contain pages that are present in the document.
        /// 
        /// This is not necessary and some apps don't do this validation. Instead, they just ignore the pages
        /// that are not present in the document and simply don't print them.
        /// </summary>
        /// <param name="pageRangeOption">The PrintPageRangeOptionDetails option</param>
        void ValidatePageRangeOption(Windows::Graphics::Printing::OptionDetails::PrintPageRangeOptionDetails^ pageRangeOption);

        /// <summary>
        /// This is the event handler for PrintDocument.AddPages. It provides all pages to be printed, in the form of
        /// UIElements, to an instance of PrintDocument. PrintDocument subsequently converts the UIElements
        /// into a pages that the Windows print system can deal with.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e">Add page event arguments containing a print task options reference</param>
        virtual void AddPrintPages(Platform::Object^ sender, Windows::UI::Xaml::Printing::AddPagesEventArgs^ e) override;

    private:

        /// <summary>
        /// Option change event handler
        /// </summary>
        /// <param name="sender">PrintTaskOptionsDetails object</param>
        /// <param name="args">the event arguments containing the changed option id</param>
        void printDetailedOptions_OptionChanged(Windows::Graphics::Printing::OptionDetails::PrintTaskOptionDetails^ sender,
            Windows::Graphics::Printing::OptionDetails::PrintTaskOptionChangedEventArgs^ args);

        /// <summary>
        /// Helper function used to triger a preview refresh
        /// </summary>
        void RefreshPreview();
    };

    /// <summary>
    /// Scenario that demonstrates a page range implementation
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario4PageRange sealed
    {
    public:
        Scenario4PageRange();
        property Windows::UI::Xaml::Controls::Frame^ PrintFrame;

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        PrintHelper^ printHelper;
        void OnPrintButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
