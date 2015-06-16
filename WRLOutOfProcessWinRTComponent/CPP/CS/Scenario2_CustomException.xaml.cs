//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using Microsoft.SDKSamples.Kitchen;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;

namespace WRLOutOfProcessWinRTComponent
{
    public sealed partial class CustomException : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public CustomException()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }

        /// <summary>
        /// Handle a Windows Runtime error originated by the Oven Windows Runtime component.
        /// </summary>
        /// <param name="sender">Contains information about the button that fired the event.</param>
        /// <param name="e">Contains state information and event data associated with a routed event.</param>
        private void Start_Click(object sender, RoutedEventArgs e)
        {
            Oven myOven = new Oven();

            try
            {
                // Intentionally pass a bad parameter
                myOven.ConfigurePreheatTemperature((OvenTemperature)5);
            }
            catch (System.ArgumentException)
            {
                CustomExceptionOutput.Text += "Exception caught. Please attach a debugger and enable first chance native exceptions to view exception details.\n";
            }
        }
    }
}
