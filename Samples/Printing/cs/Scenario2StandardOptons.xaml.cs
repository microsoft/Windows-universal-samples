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
using Windows.Graphics.Printing;
using SDKTemplate;

namespace PrintSample
{
    class StandardOptionsPrintHelper : PrintHelper
    {
        public StandardOptionsPrintHelper(Page scenarioPage) : base(scenarioPage) { }

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
                IList<string> displayedOptions = printTask.Options.DisplayedOptions;

                // Choose the printer options to be shown.
                // The order in which the options are appended determines the order in which they appear in the UI
                displayedOptions.Clear();
                displayedOptions.Add(Windows.Graphics.Printing.StandardPrintTaskOptions.Copies);
                displayedOptions.Add(Windows.Graphics.Printing.StandardPrintTaskOptions.Orientation);
                displayedOptions.Add(Windows.Graphics.Printing.StandardPrintTaskOptions.MediaSize);
                displayedOptions.Add(Windows.Graphics.Printing.StandardPrintTaskOptions.Collation);
                displayedOptions.Add(Windows.Graphics.Printing.StandardPrintTaskOptions.Duplex);

                // Preset the default value of the printer option
                printTask.Options.MediaSize = PrintMediaSize.NorthAmericaLegal;

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
    }

    /// <summary>
    /// Scenario that demos how to add customizations in the displayed options list.
    /// </summary>
    public sealed partial class Scenario2StandardOptions : Page
    {
        private StandardOptionsPrintHelper printHelper;

        public Scenario2StandardOptions()
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
            printHelper = new StandardOptionsPrintHelper(this);
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
