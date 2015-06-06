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
        PageRangePrintHelper(Page^ scenarioPage)
            : PrintHelper(scenarioPage),
            m_totalPages(0)
        { }

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
        /// Removes pages that are not in the given range
        /// </summary>
        /// <param name="sender">The list of preview pages</param>
        /// <param name="e"></param>
        /// <note> Handling preview for page range
        /// Developers have the control over how the preview should look when the user specifies a valid page range.
        /// There are three common ways to handle this:
        /// 1) Preview remains unaffected by the page range and all the pages are shown independent of the specified page range.
        /// 2) Preview is changed and only the pages specified in the range are shown to the user.
        /// 3) Preview is changed, showing all the pages and graying out the pages not in page range.
        /// We chose option (2) for this sample, developers can choose their preview option.
        /// </note>
        virtual void OnContentCreated() override;

        /// <summary>
        /// This function creates and adds one print preview page to the internal cache of print preview
        /// pages stored in m_printPreviewPages.
        /// </summary>
        /// <param name="lastRTBOAdded">Last RichTextBlockOverflow element added in the current content</param>
        /// <param name="printPageDescription">Printer's page description</param>
        virtual RichTextBlockOverflow^ AddOnePrintPreviewPage(RichTextBlockOverflow^ lastRTBOAdded, Windows::Graphics::Printing::PrintPageDescription printPageDescription) override;

    private:
        /// <summary>
        /// Determines if the Page Range Edit option is visible
        /// </summary>
        bool m_pageRangeEditVisible;

        /// <summary>
        /// Collection used to hold the pages in the specified range option
        /// </summary>
        std::vector<int> m_pageList;

        /// <summary>
        /// Number of total pages determined by the current combination of settings (without page range)
        /// </summary>
        size_t m_totalPages;

        /// <summary>
        /// Flag used to determine if content selection mode is on
        /// </summary>
        bool m_selectionMode;

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

        /// <summary>
        /// Helper function used to split a string based on some delimiters
        /// </summary>
        /// <param name="string">String to be splited</param>
        /// <param name="delimiter">Delimiter character used to split the string</param>
        /// <param name="words">Splited (output)words</param>
        void SplitString(Platform::String^ string, wchar_t delimiter, std::vector<std::wstring>& words);

        /// <summary>
        /// This is where we parse the range field
        /// </summary>
        /// <param name="pageRange">the page range value</param>
        void GetPagesInRange(Platform::String^ pageRange);

        /// <summary>
        /// Removes the PageRange edit from the charm window
        /// </summary>
        /// <param name="printTaskOptionDetails">Details regarding PrintTaskOptions</param>
        void RemovePageRangeEdit(Windows::Graphics::Printing::OptionDetails::PrintTaskOptionDetails^ printTaskOptionDetails);

        void ShowContent(Platform::String^ selectionText);
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
