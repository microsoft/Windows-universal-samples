// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario4PageRange.xaml.h"
#include "PageToPrint.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Graphics::Printing;
using namespace Windows::Graphics::Printing::OptionDetails;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

PageRangePrintHelper::PageRangePrintHelper(Page^ scenarioPage)
    : PrintHelper(scenarioPage)
{
    // Use moderate margins to allow the content to span multiple pages
    ApplicationContentMarginLeft = 0.1F;
    ApplicationContentMarginTop = 0.1F;
}

/// <summary>
/// This is the event handler for PrintManager.PrintTaskRequested.
/// In order to ensure a good user experience, the system requires that the app handle the PrintTaskRequested event within the time specified by PrintTaskRequestedEventArgs->Request->Deadline.
/// Therefore, we use this handler to only create the print task.
/// The print settings customization can be done when the print document source is requested.
/// </summary>
/// <param name="sender">PrintManager</param>
/// <param name="e">PrintTaskRequestedEventArgs</param>
void PageRangePrintHelper::PrintTaskRequested(PrintManager^ sender, PrintTaskRequestedEventArgs^ e)
{
    PrintTask^ printTask = e->Request->CreatePrintTask(L"C++ Printing SDK Sample", ref new PrintTaskSourceRequestedHandler([=](PrintTaskSourceRequestedArgs^ args)
    {
        args->SetSource(PrintDocumentSource);
    }));

    // Choose the printer options to be shown.
    // The order in which the options are appended determines the order in which they appear in the UI
    PrintTaskOptionDetails^ printDetailedOptions = PrintTaskOptionDetails::GetFromPrintTaskOptions(printTask->Options);
    IVector<String^>^ displayedOptions = printDetailedOptions->DisplayedOptions;
    displayedOptions->Clear();

    displayedOptions->Append(StandardPrintTaskOptions::Copies);
    displayedOptions->Append(StandardPrintTaskOptions::CustomPageRanges);
    displayedOptions->Append(StandardPrintTaskOptions::MediaSize);
    displayedOptions->Append(StandardPrintTaskOptions::ColorMode);

    printTask->Options->PageRangeOptions->AllowAllPages = true;
    printTask->Options->PageRangeOptions->AllowCurrentPage = true;
    printTask->Options->PageRangeOptions->AllowCustomSetOfPages = true;

    // Register the handler for the option change event
    printDetailedOptions->OptionChanged += ref new TypedEventHandler<PrintTaskOptionDetails^, PrintTaskOptionChangedEventArgs^>(this, &PageRangePrintHelper::printDetailedOptions_OptionChanged);

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

            ScenarioPage->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, callback);
        }
    });
}

/// <summary>
/// Option change event handler
/// </summary>
/// <param name="sender">PrintTaskOptionsDetails object</param>
/// <param name="args">the event arguments containing the changed option id</param>
void PageRangePrintHelper::printDetailedOptions_OptionChanged(PrintTaskOptionDetails^ sender, PrintTaskOptionChangedEventArgs^ args)
{
    if (args->OptionId == nullptr)
    {
        return;
    }

    String^ optionId = args->OptionId->ToString();

    // Handle change in Page Range Option
    if (optionId == StandardPrintTaskOptions::CustomPageRanges)
    {
        PrintPageRangeOptionDetails^ pageRangeOption = (PrintPageRangeOptionDetails^)sender->Options->Lookup(optionId);
        ValidatePageRangeOption(pageRangeOption);
    }
}

/// <summary>
/// This is the event handler for PrintDocument.AddPages. It provides all pages to be printed, in the form of
/// UIElements, to an instance of PrintDocument. PrintDocument subsequently converts the UIElements
/// into a pages that the Windows print system can deal with.
/// </summary>
/// <param name="sender"></param>
/// <param name="e">Add page event arguments containing a print task options reference</param>
void PageRangePrintHelper::AddPrintPages(Object^ sender, AddPagesEventArgs^ e)
{
    IVectorView<PrintPageRange^>^ customPageRanges = e->PrintTaskOptions->CustomPageRanges->GetView();
    PrintDocument^ printDocument = safe_cast<PrintDocument^>(sender);

    // An empty CustomPageRanges means "All Pages"
    if (customPageRanges->Size == 0)
    {
        // Loop over all of the preview pages and add each one to be printed
        for (UIElement^ previewPage : PrintPreviewPages)
        {
            printDocument->AddPage(previewPage);
        }
    }
    else
    {
        // Print only the pages chosen by the user.
        // 
        // The "Current page" option is a special case of "Custom set of pages".
        // In case the user selects the "Current page" option, the PrintDialog
        // will turn that into a CustomPageRanges containing the page that the user was looking at.
        // If the user typed in an indefinite range such as "6-", the LastPageNumber value
        // will be whatever this sample app last passed into the PrintDocument->SetPreviewPageCount API.
        for (PrintPageRange^ pageRange : customPageRanges)
        {
            // The user may type in a page number that is not present in the document.
            // In this case, we just ignore those pages, hence the checks
            // (pageRange.FirstPageNumber <= printPreviewPages.Count) and (i <= printPreviewPages.Count).
            //
            // If the user types the same page multiple times, it will be printed multiple times
            // (e.g 3-4;1;1 will print pages 3 and 4 followed by two copies of page 1)
            if (pageRange->FirstPageNumber <= PrintPreviewPageCount)
            {
                for (int i = pageRange->FirstPageNumber; (i <= pageRange->LastPageNumber) && (i <= PrintPreviewPageCount); i++)
                {
                    // Subtract 1 because page numbers are 1-based, but our list is 0-based.
                    printDocument->AddPage(PrintPreviewPages.at(i - 1));
                }
            }
        }
    }

    // Indicate that all of the print pages have been provided
    printDocument->AddPagesComplete();
}

/// <summary>
/// If the preview pages are regenerated, we need to make validate the Page Ranges again
/// </summary>
/// <param name="printTaskOptions"></param>
void PageRangePrintHelper::OnPreviewPagesCreated(PrintTaskOptions^ printTaskOptions)
{
    PrintTaskOptionDetails^ printDetailedOptions = PrintTaskOptionDetails::GetFromPrintTaskOptions(printTaskOptions);
    PrintPageRangeOptionDetails^ pageRangeOption = (PrintPageRangeOptionDetails^)printDetailedOptions->Options->Lookup(StandardPrintTaskOptions::CustomPageRanges);
    ValidatePageRangeOption(pageRangeOption);
}

/// <summary>
/// Deals with validating that the Page Ranges only contain pages that are present in the document.
/// 
/// This is not necessary and some apps don't do this validation. Instead, they just ignore the pages
/// that are not present in the document and simply don't print them.
/// </summary>
/// <param name="pageRangeOption">The PrintPageRangeOptionDetails option</param>
void PageRangePrintHelper::ValidatePageRangeOption(PrintPageRangeOptionDetails^ pageRangeOption)
{
    IVectorView<PrintPageRange^>^ pageRanges = ((IVector<PrintPageRange^>^)pageRangeOption->Value)->GetView();

    pageRangeOption->WarningText = "";
    pageRangeOption->ErrorText = "";

    int currentPreviewPageCount = PrintPreviewPageCount;

    // An empty list means AllPages, in which case we don't need to check the ranges
    if (pageRanges->Size > 0)
    {
        // Verify that the page ranges contain at least one printable page
        bool containsAtLeastOnePrintablePage = false;
        for (PrintPageRange^ pageRange : pageRanges)
        {
            if ((pageRange->FirstPageNumber <= currentPreviewPageCount) || (pageRange->LastPageNumber <= currentPreviewPageCount))
            {
                containsAtLeastOnePrintablePage = true;
                break;
            }
        }

        if (containsAtLeastOnePrintablePage)
        {
            // Warn the user in case one of the page ranges contains pages that will not printed. That way, s/he
            // can fix the page numbers in case they were mistyped.
            for (PrintPageRange^ pageRange : pageRanges)
            {
                if ((pageRange->FirstPageNumber > currentPreviewPageCount) || (pageRange->LastPageNumber > currentPreviewPageCount))
                {
                    pageRangeOption->WarningText = "One of the ranges contains pages that are not present in the document";
                    break;
                }
            }
        }
        else
        {
            pageRangeOption->ErrorText = "Those pages are not present in the document";
        }
    }
}

/// <summary>
/// Helper function used to triger a preview refresh
/// </summary>
void PageRangePrintHelper::RefreshPreview()
{
    auto callback = ref new Windows::UI::Core::DispatchedHandler([this]()
    {
        CurrentPrintDocument->InvalidatePreview();
    });

    ScenarioPage->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, callback);
}

Scenario4PageRange::Scenario4PageRange()
{
    InitializeComponent();
}

void Scenario4PageRange::OnPrintButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    printHelper->ShowPrintUIAsync();
}

void Scenario4PageRange::OnNavigatedTo(NavigationEventArgs^ e)
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
    printHelper = ref new PageRangePrintHelper(this);
    printHelper->RegisterForPrinting();

    // Initialize print content for this scenario
    printHelper->PreparePrintContent(ref new PageToPrint());
}

void Scenario4PageRange::OnNavigatedFrom(NavigationEventArgs^ e)
{
    if (printHelper)
    {
        printHelper->UnregisterForPrinting();
    }
}
