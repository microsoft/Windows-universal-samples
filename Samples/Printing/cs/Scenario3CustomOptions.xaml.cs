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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Printing;
using Windows.Graphics.Printing;
using Windows.Graphics.Printing.OptionDetails;
using SDKTemplate;

namespace PrintSample
{
    [Flags]
    internal enum DisplayContent : int
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
    }

    class CustomOptionsPrintHelper : PrintHelper
    {
        /// <summary>
        /// A flag that determines if text & images are to be shown
        /// </summary>
        internal DisplayContent imageText = DisplayContent.TextAndImages;

        /// <summary>
        /// Helper getter for text showing
        /// </summary>
        private bool ShowText
        {
            get { return (imageText & DisplayContent.Text) == DisplayContent.Text; }
        }

        /// <summary>
        /// Helper getter for image showing
        /// </summary>
        private bool ShowImage
        {
            get { return (imageText & DisplayContent.Images) == DisplayContent.Images; }
        }

        public CustomOptionsPrintHelper(Page scenarioPage) : base(scenarioPage) { }

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
                PrintCustomItemListOptionDetails pageFormat = printDetailedOptions.CreateItemListOption("PageContent", "Pictures");
                pageFormat.AddItem("PicturesText", "Pictures and text");
                pageFormat.AddItem("PicturesOnly", "Pictures only");
                pageFormat.AddItem("TextOnly", "Text only");

                // Add the custom option to the option list
                displayedOptions.Add("PageContent");

                printDetailedOptions.OptionChanged += printDetailedOptions_OptionChanged;

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

        async void printDetailedOptions_OptionChanged(PrintTaskOptionDetails sender, PrintTaskOptionChangedEventArgs args)
        {
            // Listen for PageContent changes
            string optionId = args.OptionId as string;
            if (string.IsNullOrEmpty(optionId))
            {
                return;
            }

            if (optionId == "PageContent")
            {
                await scenarioPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                {
                    printDocument.InvalidatePreview();
                });
            }
        }

        #region PrintPreview

        protected override RichTextBlockOverflow AddOnePrintPreviewPage(RichTextBlockOverflow lastRTBOAdded, PrintPageDescription printPageDescription)
        {
            // Check if we need to hide/show text & images for this scenario
            // Since all is rulled by the first page (page flow), here is where we must start
            if (lastRTBOAdded == null)
            {
                // Get a refference to page objects
                Grid pageContent = (Grid)firstPage.FindName("PrintableArea");
                Image scenarioImage = (Image)firstPage.FindName("ScenarioImage");
                RichTextBlock mainText = (RichTextBlock)firstPage.FindName("TextContent");
                RichTextBlockOverflow firstLink = (RichTextBlockOverflow)firstPage.FindName("FirstLinkedContainer");
                RichTextBlockOverflow continuationLink = (RichTextBlockOverflow)firstPage.FindName("ContinuationPageLinkedContainer");

                // Hide(collapse) and move elements in different grid cells depending by the viewable content(only text, only pictures)

                scenarioImage.Visibility = ShowImage ? Windows.UI.Xaml.Visibility.Visible : Windows.UI.Xaml.Visibility.Collapsed;
                firstLink.SetValue(Grid.ColumnSpanProperty, ShowImage ? 1 : 2);

                scenarioImage.SetValue(Grid.RowProperty, ShowText ? 2 : 1);
                scenarioImage.SetValue(Grid.ColumnProperty, ShowText ? 1 : 0);

                pageContent.ColumnDefinitions[0].Width = new GridLength(ShowText ? 6 : 4, GridUnitType.Star);
                pageContent.ColumnDefinitions[1].Width = new GridLength(ShowText ? 4 : 6, GridUnitType.Star);

                // Break the text flow if the app is not printing text in order not to spawn pages with blank content
                mainText.Visibility = ShowText ? Windows.UI.Xaml.Visibility.Visible : Windows.UI.Xaml.Visibility.Collapsed;
                continuationLink.Visibility = ShowText ? Windows.UI.Xaml.Visibility.Visible : Windows.UI.Xaml.Visibility.Collapsed;

                // Hide header if printing only images
                StackPanel header = (StackPanel)firstPage.FindName("Header");
                header.Visibility = ShowText ? Windows.UI.Xaml.Visibility.Visible : Windows.UI.Xaml.Visibility.Collapsed;
            }

            //Continue with the rest of the base printing layout processing (paper size, printable page size)
            return base.AddOnePrintPreviewPage(lastRTBOAdded, printPageDescription);
        }

        protected override void CreatePrintPreviewPages(object sender, PaginateEventArgs e)
        {
            // Get PageContent property
            PrintTaskOptionDetails printDetailedOptions = PrintTaskOptionDetails.GetFromPrintTaskOptions(e.PrintTaskOptions);
            string pageContent = (printDetailedOptions.Options["PageContent"].Value as string).ToLowerInvariant();

            // Set the text & image display flag
            imageText = (DisplayContent)((Convert.ToInt32(pageContent.Contains("pictures")) << 1) | Convert.ToInt32(pageContent.Contains("text")));

            base.CreatePrintPreviewPages(sender, e);
        }

        #endregion

    }

    /// <summary>
    /// Scenario that demos how to add custom options (specific for the application)
    /// </summary>
    public sealed partial class Scenario3CustomOptions : Page
    {
        private CustomOptionsPrintHelper printHelper;

        public Scenario3CustomOptions()
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
            printHelper = new CustomOptionsPrintHelper(this);
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
