// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario6DisablePreview.xaml.h"
#include "PageToPrint.xaml.h"
#include "ContinuationPage.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation::Metadata;
using namespace Windows::Graphics::Display;
using namespace Windows::Graphics::Printing;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;


void PreviewOptionsPrintHelper::PrintTaskRequested(PrintManager^ sender, PrintTaskRequestedEventArgs^ e)
{
    PrintTask^ printTask = e->Request->CreatePrintTask("C++ Printing SDK Sample", ref new PrintTaskSourceRequestedHandler([=](PrintTaskSourceRequestedArgs^ args)
    {
        args->SetSource(PrintDocumentSource);
    }));

    // Print Task event handler is invoked when the print job is completed.
    printTask->Completed += ref new TypedEventHandler<PrintTask^, PrintTaskCompletedEventArgs^>([=](PrintTask^ sender, PrintTaskCompletedEventArgs^ e)
    {
        // Notify the user when the print operation fails.
        if (e->Completion == PrintTaskCompletion::Failed)
        {
            auto callback = ref new DispatchedHandler([=]()
            {
                MainPage::Current->NotifyUser(ref new String(L"Failed to print."), NotifyType::ErrorMessage);
            });

            ScenarioPage->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, callback);
        }
    });

    // Choose not to show the preview by setting the property on PrintTask
    printTask->IsPreviewEnabled = false;
}

void PreviewOptionsPrintHelper::AddPrintPages(Object^ sender, AddPagesEventArgs^ e)
{
    // Clear the cache of print pages
    m_printPages.clear();

    // Clear the printing root of print pages
    PrintCanvas->Children->Clear();

    // This variable keeps track of the last RichTextBlockOverflow element that was added to a page which will be printed
    RichTextBlockOverflow^ lastRTBOOnPage;

    // Get the PrintTaskOptions
    PrintTaskOptions^ printingOptions = safe_cast<PrintTaskOptions^>(e->PrintTaskOptions);

    // Get the page description to deterimine how big the page is
    PrintPageDescription pageDescription = printingOptions->GetPageDescription(0);

    // We know there is at least one page to be printed. passing null as the first parameter to
    // AddOnePrintPage tells the function to add the first page.
    lastRTBOOnPage = AddOnePrintPage(nullptr, pageDescription);

    // We know there are more pages to be added as long as the last RichTextBoxOverflow added to a print
    // page has extra content
    while (lastRTBOOnPage->HasOverflowContent && (lastRTBOOnPage->Visibility == Visibility::Visible))
    {
        lastRTBOOnPage = AddOnePrintPage(lastRTBOOnPage, pageDescription);
    }

    PrintDocument^ printDocument = safe_cast<PrintDocument^>(sender);

    // Loop over all of the print pages and add each one to  add each page to be printed

    for (int i = 0; i < m_printPages.size(); i++)
    {
        printDocument->AddPage(m_printPages[i]);
    }

    // Indicate that all of the print pages have been provided
    printDocument->AddPagesComplete();
}

RichTextBlockOverflow^ PreviewOptionsPrintHelper::AddOnePrintPage(RichTextBlockOverflow^ lastRTBOAdded, PrintPageDescription printPageDescription)
{
    // XAML element that is used to represent to "printing page"
    FrameworkElement^ page;

    // The link container for text overflowing in this page
    RichTextBlockOverflow^ textLink;

    // Check if this is the first page ( no previous RichTextBlockOverflow)
    if (lastRTBOAdded == nullptr)
    {
        // If this is the first page add the specific scenario content
        page = FirstPage;
        //Hide footer since we don't know yet if it will be displayed (this might not be the last page) - wait for layout
        StackPanel^ footer = safe_cast<StackPanel^>(page->FindName("Footer"));
        footer->Visibility = Visibility::Collapsed;
    }
    else
    {
        // Flow content (text) from previous pages
        page = ref new ContinuationPage(lastRTBOAdded);
    }

    // Set "paper" width
    page->Width = printPageDescription.PageSize.Width;
    page->Height = printPageDescription.PageSize.Height;

    Grid^ printableArea = safe_cast<Grid^>(page->FindName("PrintableArea"));

    // Get the margins size
    // If the ImageableRect is smaller than the app provided margins use the ImageableRect
    double marginWidth = (std::max)(printPageDescription.PageSize.Width - printPageDescription.ImageableRect.Width, printPageDescription.PageSize.Width * ApplicationContentMarginLeft * 2);
    double marginHeight = (std::max)(printPageDescription.PageSize.Height - printPageDescription.ImageableRect.Height, printPageDescription.PageSize.Height * ApplicationContentMarginTop * 2);

    // Set-up "printable area" on the "paper"
    printableArea->Width = FirstPage->Width - marginWidth;
    printableArea->Height = FirstPage->Height - marginHeight;

    // Add the (newly created) page to the printing root which is part of the visual tree and force it to go
    // through layout so that the linked containers correctly distribute the content inside them.
    PrintCanvas->Children->Append(page);
    PrintCanvas->InvalidateMeasure();
    PrintCanvas->UpdateLayout();

    // Find the last text container and see if the content is overflowing
    textLink = safe_cast<RichTextBlockOverflow^>(page->FindName("ContinuationPageLinkedContainer"));

    // Check if this is the last page
    if ((!textLink->HasOverflowContent) && (textLink->Visibility == Visibility::Visible))
    {
        StackPanel^ footer = safe_cast<StackPanel^>(page->FindName("Footer"));
        footer->Visibility = Visibility::Visible;
    }

    // Add the page to the page print collection
    m_printPages.push_back(page);

    return textLink;
}

Scenario6DisablePreview::Scenario6DisablePreview()
{
    InitializeComponent();
}

void Scenario6DisablePreview::OnPrintButtonClick(Object^ sender, RoutedEventArgs^ e)
{
    printHelper->ShowPrintUIAsync();
}

void Scenario6DisablePreview::OnNavigatedTo(NavigationEventArgs^ e)
{
    if (PrintManager::IsSupported())
    {
        // Tell the user how to print
        MainPage::Current->NotifyUser("Print contract registered with customization, use the Print button to print.", NotifyType::StatusMessage);
    }
    else
    {
        // Remove the print button
        InvokePrintingButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

        // Inform user that Printing is not supported
        MainPage::Current->NotifyUser("Printing is not supported.", NotifyType::ErrorMessage);

        // Printing-related event handlers will never be called if printing
        // is not supported, but it's okay to register for them anyway.
    }

    // Initalize common helper class and register for printing
    printHelper = ref new PreviewOptionsPrintHelper(this);
    printHelper->RegisterForPrinting();

    // Initialize print content for this scenario
    printHelper->PreparePrintContent(ref new PageToPrint());
}

void Scenario6DisablePreview::OnNavigatedFrom(NavigationEventArgs^ e)
{
    if (printHelper != nullptr)
    {
        printHelper->UnregisterForPrinting();
    }
}
