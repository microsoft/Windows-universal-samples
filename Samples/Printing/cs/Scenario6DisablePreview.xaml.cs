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

using SDKTemplate;
using System;
using System.Collections.Generic;
using Windows.Foundation.Metadata;
using Windows.Graphics.Printing;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Printing;


namespace PrintSample
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
    class DisablePreviewPrintHelper : PrintHelper
    {
        /// <summary>
        /// A list of UIElements used to store the print pages.  This gives easy access
        /// to any desired print page.
        /// </summary>
        internal List<UIElement> printPages;

        public DisablePreviewPrintHelper(Page scenarioPage) : base(scenarioPage) {
            printPages = new List<UIElement>();
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
                // Print Task event handler is invoked when the print job is completed.
                printTask.Completed += async (s, args) =>
                {
                    // Notify the user when the print operation fails.
                    if (args.Completion == PrintTaskCompletion.Failed)
                    {
                        await scenarioPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                        {
                            MainPage.Current.NotifyUser("Failed to print.", NotifyType.ErrorMessage);
                        });
                    }
                };

                sourceRequestedArgs.SetSource(printDocumentSource);
            });

            // Choose not to show the preview by setting the property on PrintTask
            printTask.IsPreviewEnabled = false;
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
            // Clear the cache of print pages
            printPages.Clear();

            // Clear the print canvas of print pages
            PrintCanvas.Children.Clear();

            // This variable keeps track of the last RichTextBlockOverflow element that was added to a page which will be printed
            RichTextBlockOverflow lastRTBOOnPage;

            // Get the PrintTaskOptions
            PrintTaskOptions printingOptions = ((PrintTaskOptions)e.PrintTaskOptions);

            // Get the page description to deterimine how big the page is
            PrintPageDescription pageDescription = printingOptions.GetPageDescription(0);

            // We know there is at least one page to be printed. passing null as the first parameter to
            // AddOnePrintPage tells the function to add the first page.
            lastRTBOOnPage = AddOnePrintPage(null, pageDescription);

            // We know there are more pages to be added as long as the last RichTextBoxOverflow added to a print
            // page has extra content
            while (lastRTBOOnPage.HasOverflowContent && (lastRTBOOnPage.Visibility == Windows.UI.Xaml.Visibility.Visible))
            {
                lastRTBOOnPage = AddOnePrintPage(lastRTBOOnPage, pageDescription);
            }

            // Loop over all of the pages and add each one to add each page to be printed
            for (int i = 0; i < printPages.Count; i++)
            {
                // We should have all pages ready at this point...
                printDocument.AddPage(printPages[i]);
            }

            PrintDocument printDoc = (PrintDocument)sender;

            // Indicate that all of the print pages have been provided
            printDoc.AddPagesComplete();
        }

        /// <summary>
        /// This function creates and adds one print page to the internal cache of print pages
        /// pages stored in printPages.
        /// </summary>
        /// <param name="lastRTBOAdded">Last RichTextBlockOverflow element added in the current content</param>
        /// <param name="printPageDescription">Printer's page description</param>
        protected RichTextBlockOverflow AddOnePrintPage(RichTextBlockOverflow lastRTBOAdded, PrintPageDescription printPageDescription)
        {
            // XAML element that is used to represent to "printing page"
            FrameworkElement page;

            // The link container for text overflowing in this page
            RichTextBlockOverflow textLink;

            // Check if this is the first page ( no previous RichTextBlockOverflow)
            if (lastRTBOAdded == null)
            {
                // If this is the first page add the specific scenario content
                page = firstPage;
                //Hide footer since we don't know yet if it will be displayed (this might not be the last page) - wait for layout
                StackPanel footer = (StackPanel)page.FindName("Footer");
                footer.Visibility = Visibility.Collapsed;
            }
            else
            {
                // Flow content (text) from previous pages
                page = new ContinuationPage(lastRTBOAdded);
            }

            // Set "paper" width
            page.Width = printPageDescription.PageSize.Width;
            page.Height = printPageDescription.PageSize.Height;

            Grid printableArea = (Grid)page.FindName("PrintableArea");

            // Get the margins size
            // If the ImageableRect is smaller than the app provided margins use the ImageableRect
            double marginWidth = Math.Max(printPageDescription.PageSize.Width - printPageDescription.ImageableRect.Width, printPageDescription.PageSize.Width * ApplicationContentMarginLeft * 2);
            double marginHeight = Math.Max(printPageDescription.PageSize.Height - printPageDescription.ImageableRect.Height, printPageDescription.PageSize.Height * ApplicationContentMarginTop * 2);

            // Set-up "printable area" on the "paper"
            printableArea.Width = firstPage.Width - marginWidth;
            printableArea.Height = firstPage.Height - marginHeight;

            // Add the (newly created) page to the print canvas which is part of the visual tree and force it to go
            // through layout so that the linked containers correctly distribute the content inside them.
            PrintCanvas.Children.Add(page);
            PrintCanvas.InvalidateMeasure();
            PrintCanvas.UpdateLayout();

            // Find the last text container and see if the content is overflowing
            textLink = (RichTextBlockOverflow)page.FindName("ContinuationPageLinkedContainer");

            // Check if this is the last page
            if ((!textLink.HasOverflowContent) && (textLink.Visibility == Windows.UI.Xaml.Visibility.Visible))
            {
                StackPanel footer = (StackPanel)page.FindName("Footer");
                footer.Visibility = Visibility.Visible;
            }

            // Add the page to the print page collection
            printPages.Add(page);

            return textLink;
        }
    }

    /// <summary>
    /// Scenario that demos how to disable the print preview in the Modern Print Dialog window.
    /// </summary>
    public sealed partial class Scenario6DisablePreview : Page
    {
        private DisablePreviewPrintHelper printHelper;

        public Scenario6DisablePreview()
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
            printHelper = new DisablePreviewPrintHelper(this);
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
