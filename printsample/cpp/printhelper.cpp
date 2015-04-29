// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "PrintHelper.h"
#include "ContinuationPage.xaml.h"

using namespace SDKTemplate;

using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::Graphics::Printing;
using namespace Windows::UI;
using namespace Windows::UI::Text;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;
using namespace Platform;

PrintHelper::PrintHelper(Page^ scenarioPage)
{
    m_scenarioPage = scenarioPage;
}

/// <summary>
/// This function registers the app for printing with Windows and sets up the necessary event handlers for the print process.
/// </summary>
void PrintHelper::RegisterForPrinting()
{
    m_printDocument = ref new PrintDocument();
    m_printDocumentSource = m_printDocument->DocumentSource;
    m_printDocument->Paginate += ref new Windows::UI::Xaml::Printing::PaginateEventHandler(this, &PrintHelper::CreatePrintPreviewPages);
    m_printDocument->GetPreviewPage += ref new Windows::UI::Xaml::Printing::GetPreviewPageEventHandler(this, &PrintHelper::GetPrintPreviewPage);
    m_printDocument->AddPages += ref new Windows::UI::Xaml::Printing::AddPagesEventHandler(this, &PrintHelper::AddPrintPages);

    PrintManager^ printMan = PrintManager::GetForCurrentView();
    m_printTaskRequestedEventToken = printMan->PrintTaskRequested += ref new TypedEventHandler<PrintManager^, PrintTaskRequestedEventArgs^>(this, &PrintHelper::PrintTaskRequested);
}

void PrintHelper::UnregisterForPrinting()
{
    // Remove the handler for printing initialization.
    PrintManager^ printMan = PrintManager::GetForCurrentView();
    printMan->PrintTaskRequested -= m_printTaskRequestedEventToken;

    PrintCanvas->Children->Clear();
}

void PrintHelper::ShowPrintUIAsync()
{
    concurrency::create_task(PrintManager::ShowPrintUIAsync()).then([=](bool succeeded)
    {
        if (!succeeded)
        {
            MainPage::Current->NotifyUser("Error showing Print Dialog", NotifyType::ErrorMessage);
        }
    }).then([=](concurrency::task<void> previousTask)
    {
        // Catch and print out any errors reported
        try
        {
            previousTask.get();
        }
        catch (Exception^ e)
        {
            MainPage::Current->NotifyUser("Error showing Print Dialog: " + e->Message + ", hr=" + e->HResult, NotifyType::ErrorMessage);
        }
    });
}

void PrintHelper::PreparePrintContent(Page^ page)
{
    PreparePrintContentImpl(page);
}

void PrintHelper::PreparePrintContentImpl(Page^ page)
{
    if (!FirstPage)
    {
        FirstPage = page;
        StackPanel^ header = safe_cast<StackPanel^>(FirstPage->FindName("Header"));
        header->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }

    // Add the (newly created) page to the print canvas which is part of the visual tree and force it to go
    // through layout so that the linked containers correctly distribute the content inside them.
    PrintCanvas->Children->Append(FirstPage);
    PrintCanvas->InvalidateMeasure();
    PrintCanvas->UpdateLayout();
}

/// <summary>
/// This is the event handler for PrintManager.PrintTaskRequested.
/// In order to ensure a good user experience, the system requires that the app handle the PrintTaskRequested event within the time specified by PrintTaskRequestedEventArgs->Request->Deadline.
/// Therefore, we use this handler to only create the print task.
/// The print settings customization can be done when the print document source is requested.
/// </summary>
/// <param name="sender">PrintManager</param>
/// <param name="e">PrintTaskRequestedEventArgs</param>
void PrintHelper::PrintTaskRequested(PrintManager^ sender, PrintTaskRequestedEventArgs^ e)
{
    PrintTask^ printTask = e->Request->CreatePrintTask("C++ Printing SDK Sample", ref new Windows::Graphics::Printing::PrintTaskSourceRequestedHandler([=](PrintTaskSourceRequestedArgs^ args)
    {
        args->SetSource(m_printDocumentSource);
    }));

    // Print Task event handler is invoked when the print job is completed.
    printTask->Completed += ref new TypedEventHandler<PrintTask^, PrintTaskCompletedEventArgs^>([=](PrintTask^ sender, PrintTaskCompletedEventArgs^ e)
    {
        // Notify the user when the print operation fails.
        if (e->Completion == Windows::Graphics::Printing::PrintTaskCompletion::Failed)
        {
            auto callback = ref new Windows::UI::Core::DispatchedHandler([=]()
            {
                MainPage::Current->NotifyUser(ref new String(L"Failed to print."), NotifyType::ErrorMessage);
            });

            m_scenarioPage->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, callback);
        }
    });
}

/// <summary>
/// This is the event handler for PrintDocument.Paginate. It creates print preview pages for the app.
/// </summary>
/// <param name="sender">PrintDocument</param>
/// <param name="e">Paginate Event Arguments</param>
void PrintHelper::CreatePrintPreviewPages(Object^ sender, PaginateEventArgs^ e)
{
    // Clear the cache of preview pages
    m_printPreviewPages.clear();

    // Clear the printing root of preview pages
    PrintCanvas->Children->Clear();

    // This variable keeps track of the last RichTextBlockOverflow element that was added to a page which will be printed
    RichTextBlockOverflow^ lastRTBOOnPage;

    // Get the PrintTaskOptions
    PrintTaskOptions^ printingOptions = safe_cast<PrintTaskOptions^>(e->PrintTaskOptions);

    // Get the page description to deterimine how big the page is
    PrintPageDescription pageDescription = printingOptions->GetPageDescription(0);

    // We know there is at least one page to be printed. passing null as the first parameter to
    // AddOnePrintPreviewPage tells the function to add the first page.
    lastRTBOOnPage = AddOnePrintPreviewPage(nullptr, pageDescription);

    // We know there are more pages to be added as long as the last RichTextBoxOverflow added to a print preview
    // page has extra content
    while (lastRTBOOnPage->HasOverflowContent && lastRTBOOnPage->Visibility == Windows::UI::Xaml::Visibility::Visible)
    {
        lastRTBOOnPage = AddOnePrintPreviewPage(lastRTBOOnPage, pageDescription);
    }

    OnContentCreated();

    PrintDocument^ printDocument = safe_cast<PrintDocument^>(sender);

    // Report the number of preview pages created
    printDocument->SetPreviewPageCount(static_cast<int>(m_printPreviewPages.size()), PreviewPageCountType::Intermediate);
}

void PrintHelper::GetPrintPreviewPage(Object^ sender, GetPreviewPageEventArgs^ e)
{
    PrintDocument^ localprintDocument = safe_cast<PrintDocument^>(sender);
    localprintDocument->SetPreviewPage(e->PageNumber, m_printPreviewPages[e->PageNumber - 1]);
}

/// <summary>
/// This is the event handler for PrintDocument.AddPages. It provides all pages to be printed, in the form of
/// UIElements, to an instance of PrintDocument. PrintDocument subsequently converts the UIElements
/// into a pages that the Windows print system can deal with.
/// </summary>
/// <param name="sender"></param>
/// <param name="e">Add page event arguments containing a print task options reference</param>
void PrintHelper::AddPrintPages(Object^ sender, AddPagesEventArgs^ e)
{
    PrintDocument^ printDocument = safe_cast<PrintDocument^>(sender);

    // Loop over all of the preview pages and add each one to  add each page to be printied
    for (size_t i = 0; i < m_printPreviewPages.size(); i++)
    {
        printDocument->AddPage(m_printPreviewPages[i]);
    }

    // Indicate that all of the print pages have been provided
    printDocument->AddPagesComplete();
}

/// <summary>
/// This function creates and adds one print preview page to the internal cache of print preview
/// pages stored in m_printPreviewPages.
/// </summary>
/// <param name="lastRTBOAdded">Last RichTextBlockOverflow element added in the current content</param>
/// <param name="printPageDescription">Printer's page description</param>
RichTextBlockOverflow^ PrintHelper::AddOnePrintPreviewPage(RichTextBlockOverflow^ lastRTBOAdded, PrintPageDescription printPageDescription)
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
        footer->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
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

    // Add the (newley created) page to the printing root which is part of the visual tree and force it to go
    // through layout so that the linked containers correctly distribute the content inside them.
    PrintCanvas->Children->Append(page);
    PrintCanvas->InvalidateMeasure();
    PrintCanvas->UpdateLayout();

    // Find the last text container and see if the content is overflowing
    textLink = safe_cast<RichTextBlockOverflow^>(page->FindName("ContinuationPageLinkedContainer"));

    // Check if this is the last page
    if (!textLink->HasOverflowContent && textLink->Visibility == Windows::UI::Xaml::Visibility::Visible)
    {
        StackPanel^ footer = safe_cast<StackPanel^>(page->FindName("Footer"));
        footer->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }

    // Add the page to the page preview collection
    m_printPreviewPages.push_back(page);

    return textLink;
}

void PrintHelper::RemovePageFromPrintPreviewCollection(int pageNumber)
{
    m_printPreviewPages.erase(m_printPreviewPages.begin() + pageNumber);
}
