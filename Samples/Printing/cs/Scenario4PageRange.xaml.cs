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
using System.Collections.Immutable;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Printing;
using Windows.Graphics.Printing;
using Windows.Graphics.Printing.OptionDetails;
using SDKTemplate;

namespace PrintSample
{
    class InvalidPageException : Exception
    {
        public InvalidPageException(string message)
            : base(message)
        {
        }
    }

    class PageRangePrintHelper : PrintHelper
    {
        public PageRangePrintHelper(Page scenarioPage) : base(scenarioPage)
        {
            // Use moderate margins to allow the content to span multiple pages
            ApplicationContentMarginTop = 0.1;
            ApplicationContentMarginLeft = 0.1;
        }

        /// <summary>
        /// This is the event handler for PrintManager.PrintTaskRequested.
        /// In order to ensure a good user experience, the system requires that the app handle the PrintTaskRequested event within the time specified by PrintTaskRequestedEventArgs.Request.Deadline.
        /// Therefore, we use this handler to only create the print task.
        /// The print settings customization can be done when the print document source is requested.
        /// </summary>
        /// <param name="sender">PrintManager</param>
        /// <param name="e">PrintTaskRequestedEventArgs</param>
        protected override void PrintTaskRequested(PrintManager sender, PrintTaskRequestedEventArgs e)
        {
            PrintTask printTask = null;
            printTask = e.Request.CreatePrintTask("C# Printing SDK Sample", sourceRequestedArgs =>
            {
                PrintTaskOptionDetails printDetailedOptions = PrintTaskOptionDetails.GetFromPrintTaskOptions(printTask.Options);
                IList<string> displayedOptions = printDetailedOptions.DisplayedOptions;

                // Choose the printer options to be shown.
                // The order in which the options are appended determines the order in which they appear in the UI
                displayedOptions.Clear();

                displayedOptions.Add(Windows.Graphics.Printing.StandardPrintTaskOptions.Copies);
                displayedOptions.Add(Windows.Graphics.Printing.StandardPrintTaskOptions.CustomPageRanges);
                displayedOptions.Add(Windows.Graphics.Printing.StandardPrintTaskOptions.Orientation);
                displayedOptions.Add(Windows.Graphics.Printing.StandardPrintTaskOptions.MediaSize);
                displayedOptions.Add(Windows.Graphics.Printing.StandardPrintTaskOptions.ColorMode);

                printTask.Options.PageRangeOptions.AllowCurrentPage = true;
                printTask.Options.PageRangeOptions.AllowAllPages = true;
                printTask.Options.PageRangeOptions.AllowCustomSetOfPages = true;

                // Register the handler for the option change event
                printDetailedOptions.OptionChanged += printDetailedOptions_OptionChanged;

                // Register the handler for the PrintTask.Completed event.
                // Print Task event handler is invoked when the print job is completed.
                printTask.Completed += async (s, args) =>
                {
                    // Notify the user when the print operation fails.
                    if (args.Completion == PrintTaskCompletion.Failed)
                    {
                        await scenarioPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                        {
                            MainPage.Current.NotifyUser("Failed to print.", NotifyType.ErrorMessage);
                        });
                    }
                };

                sourceRequestedArgs.SetSource(printDocumentSource);
            });
        }

        /// <summary>
        /// This is the event handler for PrintDocument.Paginate. It creates print preview pages for the app.
        /// </summary>
        /// <param name="sender">PrintDocument</param>
        /// <param name="e">Paginate Event Arguments</param>
        protected override void CreatePrintPreviewPages(object sender, PaginateEventArgs e)
        {
            base.CreatePrintPreviewPages(sender, e);

            PrintTaskOptionDetails printDetailedOptions = PrintTaskOptionDetails.GetFromPrintTaskOptions(e.PrintTaskOptions);
            PrintPageRangeOptionDetails pageRangeOption = (PrintPageRangeOptionDetails)printDetailedOptions.Options[StandardPrintTaskOptions.CustomPageRanges];

            // The number of pages may have been changed, so validate the Page Ranges again
            ValidatePageRangeOption(pageRangeOption);
        }

        /// <summary>
        /// This is the event handler for PrintDocument.AddPages. It provides all pages to be printed, in the form of
        /// UIElements, to an instance of PrintDocument. PrintDocument subsequently converts the UIElements
        /// into a pages that the Windows print system can deal with.
        /// </summary>
        /// <param name="sender">PrintDocument</param>
        /// <param name="e">Add page event arguments containing a print task options reference</param>
        protected override void AddPrintPages(object sender, AddPagesEventArgs e)
        {
            IList<PrintPageRange> customPageRanges = e.PrintTaskOptions.CustomPageRanges;

            // An empty CustomPageRanges means "All Pages"
            if (customPageRanges.Count == 0)
            {
                // Loop over all of the preview pages and add each one to be printed
                foreach (UIElement previewPage in printPreviewPages)
                {
                    printDocument.AddPage(previewPage);
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
                // will be whatever this sample app last passed into the PrintDocument.SetPreviewPageCount API.
                foreach (PrintPageRange pageRange in customPageRanges)
                {
                    // The user may type in a page number that is not present in the document.
                    // In this case, we just ignore those pages, hence the checks
                    // (pageRange.FirstPageNumber <= printPreviewPages.Count) and (i <= printPreviewPages.Count).
                    //
                    // If the user types the same page multiple times, it will be printed multiple times
                    // (e.g 3-4;1;1 will print pages 3 and 4 followed by two copies of page 1)
                    if (pageRange.FirstPageNumber <= printPreviewPages.Count)
                    {
                        for (int i = pageRange.FirstPageNumber; (i <= pageRange.LastPageNumber) && (i <= printPreviewPages.Count); i++)
                        {
                            // Subtract 1 because page numbers are 1-based, but our list is 0-based.
                            printDocument.AddPage(printPreviewPages[i - 1]);
                        }
                    }
                }
            }

            PrintDocument printDoc = (PrintDocument)sender;

            // Indicate that all of the print pages have been provided
            printDoc.AddPagesComplete();
        }

        /// <summary>
        /// Option change event handler
        /// </summary>
        /// <param name="sender">PrintTaskOptionsDetails object</param>
        /// <param name="args">the event arguments containing the changed option id</param>
        void printDetailedOptions_OptionChanged(PrintTaskOptionDetails sender, PrintTaskOptionChangedEventArgs args)
        {
            string optionId = args.OptionId as string;
            if (string.IsNullOrEmpty(optionId))
            {
                return;
            }

            // Handle change in Page Range Option
            if (optionId == StandardPrintTaskOptions.CustomPageRanges)
            {
                PrintPageRangeOptionDetails pageRangeOption = (PrintPageRangeOptionDetails)sender.Options[optionId];
                ValidatePageRangeOption(pageRangeOption);
            }
        }

        /// <summary>
        /// Deals with validating that the Page Ranges only contain pages that are present in the document.
        /// 
        /// This is not necessary and some apps don't do this validation. Instead, they just ignore the pages
        /// that are not present in the document and simply don't print them.
        /// </summary>
        /// <param name="pageRangeOption">The PrintPageRangeOptionDetails option</param>
        void ValidatePageRangeOption(PrintPageRangeOptionDetails pageRangeOption)
        {
            IList<PrintPageRange> pageRanges = ((IList<PrintPageRange>)pageRangeOption.Value).ToImmutableList();

            pageRangeOption.WarningText = "";
            pageRangeOption.ErrorText = "";

            // An empty list means AllPages, in which case we don't need to check the ranges
            if (pageRanges.Count > 0)
            {
                lock (printPreviewPages)
                {
                    // Verify that the page ranges contain at least one printable page
                    bool containsAtLeastOnePrintablePage = false;
                    foreach (PrintPageRange pageRange in pageRanges)
                    {
                        if ((pageRange.FirstPageNumber <= printPreviewPages.Count) || (pageRange.LastPageNumber <= printPreviewPages.Count))
                        {
                            containsAtLeastOnePrintablePage = true;
                            break;
                        }
                    }

                    if (containsAtLeastOnePrintablePage)
                    {
                        // Warn the user in case one of the page ranges contains pages that will not printed. That way, s/he
                        // can fix the page numbers in case they were mistyped.
                        foreach (PrintPageRange pageRange in pageRanges)
                        {
                            if ((pageRange.FirstPageNumber > printPreviewPages.Count) || (pageRange.LastPageNumber > printPreviewPages.Count))
                            {
                                pageRangeOption.WarningText = "One of the ranges contains pages that are not present in the document";
                                break;
                            }
                        }
                    }
                    else
                    {
                        pageRangeOption.ErrorText = "Those pages are not present in the document";
                    }
                }
            }
        }
    }

    /// <summary>
    /// Scenario that demonstrates a page range implementation
    /// </summary>
    public sealed partial class Scenario4PageRange : Page
    {
        private PageRangePrintHelper printHelper;

        public Scenario4PageRange()
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
            printHelper = new PageRangePrintHelper(this);
            printHelper.RegisterForPrinting();

            // Initialize print content for this scenario
            printHelper.PreparePrintContent(new PageToPrint());
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
