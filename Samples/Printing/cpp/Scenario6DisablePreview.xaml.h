// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario6DisablePreview.g.h"
#include "PrintHelper.h"

namespace SDKTemplate
{
    /// <summary>
    /// This class customizes the standard PrintHelper in order to disable print preview.
    /// </summary>
    /// <remarks>
    /// The PrintTaskRequestedMethod sets printTask.IsPreviewEnabled to false in order
    /// to disable print preview.
    /// Since print preview is disabled, the Paginate event will not be raised.
    /// Instead, the pages to be printed are generated in response to the AddPages event.
    /// </remarks>
    ref class PreviewOptionsPrintHelper sealed : public PrintHelper
    {
    public:
        PreviewOptionsPrintHelper(Page^ scenarioPage) : PrintHelper(scenarioPage) { }

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
        /// This is the event handler for PrintDocument.AddPages. It provides all pages to be printed, in the form of
        /// UIElements, to an instance of PrintDocument. PrintDocument subsequently converts the UIElements
        /// into a pages that the Windows print system can deal with.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e">Add page event arguments containing a print task options reference</param>
        virtual void AddPrintPages(Object^ sender, AddPagesEventArgs^ e) override;

        /// <summary>
        /// This function creates and adds one print page to the internal cache of print preview
        /// pages stored in m_printPages.
        /// </summary>
        /// <param name="lastRTBOAdded">Last RichTextBlockOverflow element added in the current content</param>
        /// <param name="printPageDescription">Printer's page description</param>
        RichTextBlockOverflow^ PreviewOptionsPrintHelper::AddOnePrintPage(RichTextBlockOverflow^ lastRTBOAdded, Windows::Graphics::Printing::PrintPageDescription printPageDescription);

    private:
        /// <summary>
        /// A list of UIElements used to store the print pages.  This gives easy access
        /// to any desired print page.
        /// </summary>
        std::vector<UIElement^> m_printPages;

    };

    /// <summary>
    /// Scenario that demos how to disable the print preview in the Modern Print Dialog window.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario6DisablePreview sealed
    {
    public:
        Scenario6DisablePreview();


    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        PrintHelper^ printHelper;
        void OnPrintButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
