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
using System.Linq;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Navigation;
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
        private bool pageRangeEditVisible = false;

        /// <summary>
        /// The pages in the range
        /// </summary>
        private List<int> pageList;

        /// <summary>
        /// Flag used to determine if content selection mode is on
        /// </summary>
        private bool selectionMode;

        /// <summary>
        /// This is the original number of pages before processing(filtering) in ScenarioInput4_PagesCreated
        /// </summary>
        private int totalPages;

        public PageRangePrintHelper(Page scenarioPage) : base(scenarioPage)
        {
            pageList = new List<int>();
            PreviewPagesCreated += PageRangePrintHelper_PagesCreated;
        }

        /// <summary>
        /// Filter pages that are not in the given range
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
        void PageRangePrintHelper_PagesCreated(object sender, EventArgs e)
        {
            totalPages = printPreviewPages.Count;

            if (pageRangeEditVisible)
            {
                // ignore page range if there are any invalid pages regarding current context
                if (!pageList.Exists(page => page > printPreviewPages.Count))
                {
                    for (int i = printPreviewPages.Count; i > 0 && pageList.Count > 0; --i)
                    {
                        if (this.pageList.Contains(i) == false)
                        {
                            printPreviewPages.RemoveAt(i - 1);
                        }
                    }
                }
            }
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
                displayedOptions.Add(Windows.Graphics.Printing.StandardPrintTaskOptions.Orientation);
                displayedOptions.Add(Windows.Graphics.Printing.StandardPrintTaskOptions.ColorMode);

                // Create a new list option
                PrintCustomItemListOptionDetails pageFormat = printDetailedOptions.CreateItemListOption("PageRange", "Page Range");
                pageFormat.AddItem("PrintAll", "Print all");
                pageFormat.AddItem("PrintSelection", "Print Selection");
                pageFormat.AddItem("PrintRange", "Print Range");

                // Add the custom option to the option list
                displayedOptions.Add("PageRange");

                // Create new edit option
                PrintCustomTextOptionDetails pageRangeEdit = printDetailedOptions.CreateTextOption("PageRangeEdit", "Range");

                // Register the handler for the option change event
                printDetailedOptions.OptionChanged += printDetailedOptions_OptionChanged;

                // Register the handler for the PrintTask.Completed event.
                // Print Task event handler is invoked when the print job is completed.
                printTask.Completed += async (s, args) =>
                {
                    pageRangeEditVisible = false;
                    selectionMode = false;
                    pageList.Clear();

                    // Notify the user when the print operation fails.
                    if (args.Completion == PrintTaskCompletion.Failed)
                    {
                        await scenarioPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                        {
                            MainPage.Current.NotifyUser("Failed to print.", NotifyType.ErrorMessage);
                        });
                    }

                    await scenarioPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                    {
                        // Restore first page to its default layout.
                        // Undo any changes made by a text selection.
                        ShowContent(null);
                    });
                };

                sourceRequestedArgs.SetSource(printDocumentSource);
            });
        }

        /// <summary>
        /// Option change event handler
        /// </summary>
        /// <param name="sender">PrintTaskOptionsDetails object</param>
        /// <param name="args">the event arguments containing the changed option id</param>
        async void printDetailedOptions_OptionChanged(PrintTaskOptionDetails sender, PrintTaskOptionChangedEventArgs args)
        {
            if (args.OptionId == null)
            {
                return;
            }

            string optionId = args.OptionId.ToString();

            // Handle change in Page Range Option
            if (optionId == "PageRange")
            {
                IPrintOptionDetails pageRange = sender.Options[optionId];
                string pageRangeValue = pageRange.Value.ToString();

                selectionMode = false;

                switch (pageRangeValue)
                {
                    case "PrintRange":
                        // Add PageRangeEdit custom option to the option list
                        sender.DisplayedOptions.Add("PageRangeEdit");
                        pageRangeEditVisible = true;
                        await scenarioPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                        {
                            ShowContent(null);
                        });
                        break;
                    case "PrintSelection":
                        await scenarioPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                        {
                            Scenario4PageRange page = (Scenario4PageRange)scenarioPage;
                            PageToPrint pageContent = (PageToPrint)page.PrintFrame.Content;
                            ShowContent(pageContent.TextContentBlock.SelectedText);
                        });
                        RemovePageRangeEdit(sender);
                        break;
                    default:
                        await scenarioPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                        {
                            ShowContent(null);
                        });
                        RemovePageRangeEdit(sender);
                        break;
                }

                Refresh();
            }
            else if (optionId == "PageRangeEdit")
            {
                IPrintOptionDetails pageRange = sender.Options[optionId];
                // Expected range format (p1,p2...)*, (p3-p9)* ...
                if (!Regex.IsMatch(pageRange.Value.ToString(), @"^\s*\d+\s*(\-\s*\d+\s*)?(\,\s*\d+\s*(\-\s*\d+\s*)?)*$"))
                {
                    pageRange.ErrorText = "Invalid Page Range (eg: 1-3, 5)";
                }
                else
                {
                    pageRange.ErrorText = string.Empty;
                    try
                    {
                        GetPagesInRange(pageRange.Value.ToString());
                        Refresh();
                    }
                    catch (InvalidPageException ipex)
                    {
                        pageRange.ErrorText = ipex.Message;
                    }
                }
            }
        }

        private void ShowContent(string selectionText)
        {
            bool hasSelection = !string.IsNullOrEmpty(selectionText);
            selectionMode = hasSelection;

            // Hide/show images depending by the selected text
            StackPanel header = (StackPanel)firstPage.FindName("Header");
            header.Visibility = hasSelection ? Windows.UI.Xaml.Visibility.Collapsed : Windows.UI.Xaml.Visibility.Visible;
            Grid pageContent = (Grid)firstPage.FindName("PrintableArea");
            pageContent.RowDefinitions[0].Height = GridLength.Auto;

            Image scenarioImage = (Image)firstPage.FindName("ScenarioImage");
            scenarioImage.Visibility = hasSelection ? Windows.UI.Xaml.Visibility.Collapsed : Windows.UI.Xaml.Visibility.Visible;

            // Expand the middle paragraph on the full page if printing only selected text
            RichTextBlockOverflow firstLink = (RichTextBlockOverflow)firstPage.FindName("FirstLinkedContainer");
            firstLink.SetValue(Grid.ColumnSpanProperty, hasSelection ? 2 : 1);

            // Clear(hide) current text and add only the selection if a selection exists
            RichTextBlock mainText = (RichTextBlock)firstPage.FindName("TextContent");

            RichTextBlock textSelection = (RichTextBlock)firstPage.FindName("TextSelection");

            // Main (default) scenario text
            mainText.Visibility = hasSelection ? Windows.UI.Xaml.Visibility.Collapsed : Windows.UI.Xaml.Visibility.Visible;
            mainText.OverflowContentTarget = hasSelection ? null : firstLink;

            // Scenario text-blocks used for displaying selection
            textSelection.OverflowContentTarget = hasSelection ? firstLink : null;
            textSelection.Visibility = hasSelection ? Windows.UI.Xaml.Visibility.Visible : Windows.UI.Xaml.Visibility.Collapsed;
            textSelection.Blocks.Clear();

            // Force the visual root to go through layout so that the linked containers correctly distribute the content inside them.
            PrintCanvas.InvalidateArrange();
            PrintCanvas.InvalidateMeasure();
            PrintCanvas.UpdateLayout();

            // Add the text selection if any
            if (hasSelection)
            {
                Run inlineText = new Run();
                inlineText.Text = selectionText;

                Paragraph paragraph = new Paragraph();
                paragraph.Inlines.Add(inlineText);

                textSelection.Blocks.Add(paragraph);
            }
        }

        protected override RichTextBlockOverflow AddOnePrintPreviewPage(RichTextBlockOverflow lastRTBOAdded, PrintPageDescription printPageDescription)
        {
            RichTextBlockOverflow textLink = base.AddOnePrintPreviewPage(lastRTBOAdded, printPageDescription);

            // Don't show footer in selection mode
            if (selectionMode)
            {
                FrameworkElement page = (FrameworkElement)printPreviewPages[printPreviewPages.Count - 1];
                StackPanel footer = (StackPanel)page.FindName("Footer");
                footer.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }

            return textLink;
        }

        /// <summary>
        /// Removes the PageRange edit from the charm window
        /// </summary>
        /// <param name="printTaskOptionDetails">Details regarding PrintTaskOptions</param>
        private void RemovePageRangeEdit(PrintTaskOptionDetails printTaskOptionDetails)
        {
            if (pageRangeEditVisible)
            {
                string lastDisplayedOption = printTaskOptionDetails.DisplayedOptions.FirstOrDefault(p => p.Contains("PageRangeEdit"));
                if (!string.IsNullOrEmpty(lastDisplayedOption))
                {
                    printTaskOptionDetails.DisplayedOptions.Remove(lastDisplayedOption);
                }
                pageRangeEditVisible = false;
            }
        }

        private async void Refresh()
        {
            // Refresh
            await scenarioPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                // Refresh preview
                printDocument.InvalidatePreview();
            });
        }

        private static readonly char[] enumerationSeparator = new char[] { ',' };
        private static readonly char[] rangeSeparator = new char[] { '-' };

        /// <summary>
        /// This is where we parse the range field
        /// </summary>
        /// <param name="pageRange">the page range value</param>
        private void GetPagesInRange(string pageRange)
        {
            string[] rangeSplit = pageRange.Split(enumerationSeparator);

            // Clear the previous values
            pageList.Clear();

            foreach (string range in rangeSplit)
            {
                // Interval
                if (range.Contains("-"))
                {
                    string[] limits = range.Split(rangeSeparator);
                    int start = int.Parse(limits[0]);
                    int end = int.Parse(limits[1]);

                    if ((start < 1) || (end > totalPages) || (start >= end))
                    {
                        throw new InvalidPageException(string.Format("Invalid page(s) in range {0} - {1}", start, end));
                    }

                    for (int i = start; i <= end; ++i)
                    {
                        pageList.Add(i);
                    }
                    continue;
                }

                // Single page
                var pageNr = int.Parse(range);

                if (pageNr < 1)
                {
                    throw new InvalidPageException(string.Format("Invalid page {0}", pageNr));
                }

                // Compare to the maximum number of available pages
                if (pageNr > totalPages)
                {
                    throw new InvalidPageException(string.Format("Invalid page {0}", pageNr));
                }

                pageList.Add(pageNr);
            }
        }
    }

    /// <summary>
    /// Scenario that demonstrates a page range implementation
    /// </summary>
    public sealed partial class Scenario4PageRange : Page
    {
        private PageRangePrintHelper printHelper;
        public Frame PrintFrame { get; set; }

        public Scenario4PageRange()
        {
            this.InitializeComponent();
            PrintFrame = PageToPrintFrame;
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
                MainPage.Current.NotifyUser("Print contract registered with customization, select some text and use the Print Selection button to print.", NotifyType.StatusMessage);
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

            PageToPrintFrame.Navigate(typeof(PageToPrint), this);

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
