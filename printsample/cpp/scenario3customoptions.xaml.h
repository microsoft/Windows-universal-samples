// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario3CustomOptions.g.h"
#include "PrintHelper.h"

namespace SDKTemplate
{
    enum DisplayContent
    {
        /// <summary>
        /// Show only text
        /// </summary>
        Text = 1,

        /// <summary>
        /// Show only images
        /// </summary>
        Images = 2,

        /// <summary>
        /// Show a combination of images and text
        /// </summary>
        TextAndImages = 3
    };

    ref class CustomOptionsPrintHelper sealed : public PrintHelper
    {
    public:
        CustomOptionsPrintHelper(Page^ scenarioPage) : PrintHelper(scenarioPage) { }

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
        /// This is the event handler for PrintDocument.Paginate. It creates print preview pages for the app.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e">Paginate Event Arguments</param>
        virtual void CreatePrintPreviewPages(Object^ sender, Windows::UI::Xaml::Printing::PaginateEventArgs^ e) override;

        /// <summary>
        /// This function creates and adds one print preview page to the internal cache of print preview
        /// pages stored in m_printPreviewPages.
        /// </summary>
        /// <param name="lastRTBOAdded">Last RichTextBlockOverflow element added in the current content</param>
        /// <param name="printPageDescription">Printer's page description</param>
        virtual Windows::UI::Xaml::Controls::RichTextBlockOverflow^ AddOnePrintPreviewPage(
            Windows::UI::Xaml::Controls::RichTextBlockOverflow^ lastRTBOAdded,
            Windows::Graphics::Printing::PrintPageDescription printPageDescription) override;

    private:
        /// <summary>
        /// A flag that determines if text & images are to be shown
        /// </summary>
        DisplayContent m_imageText;

        /// <summary>
        /// Helper getter for image showing
        /// </summary>
        property bool ShowImage
        {
            bool get()
            {
                return (m_imageText & DisplayContent::Images) == DisplayContent::Images;
            }
        }

        /// <summary>
        /// Helper getter for text showing
        /// </summary>
        property bool ShowText
        {
            bool get()
            {
                return (m_imageText & DisplayContent::Text) == DisplayContent::Text;
            }
        }

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
    /// Scenario that demos how to add custom options (specific for the application)
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3CustomOptions sealed
    {
    public:
        Scenario3CustomOptions();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        PrintHelper^ printHelper;
        void OnPrintButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
