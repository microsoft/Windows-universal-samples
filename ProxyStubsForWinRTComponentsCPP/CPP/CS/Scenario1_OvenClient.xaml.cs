//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using Microsoft.SDKSamples.Kitchen;

using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;

namespace ProxyStubsForWinRTComponents
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class OvenClient : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public OvenClient()
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
        /// Use the custom Oven and Bread Windows Runtime components. 
        /// </summary>
        /// <param name="sender">Contains information about the button that fired the event.</param>
        /// <param name="e">Contains state information and event data associated with a routed event.</param>
        private void Start_Click(object sender, RoutedEventArgs e)
        {
            Dimensions dimensions;
            dimensions.Width = 2;
            dimensions.Height = 2;
            dimensions.Depth = 2;

            // Component Creation
            Oven myOven = new Oven(dimensions);

            // Getters and setters are accessed using property syntax
            OvenClientOutput.Text += "Oven volume is: " + myOven.Volume.ToString() + "\n";

            // Register even listeners
            myOven.BreadBaked += new TypedEventHandler<Oven, Bread>(BreadCompletedHandler1);
            myOven.BreadBaked += new TypedEventHandler<Oven, Bread>(BreadCompletedHandler2);
            myOven.BreadBaked += new TypedEventHandler<Oven, Bread>(BreadCompletedHandler3);

            // Unregister from an event using the -= syntax and a delegate instance
            myOven.BreadBaked -= new TypedEventHandler<Oven, Bread>(BreadCompletedHandler3);

            // Bake a loaf of bread. This will trigger the BreadBaked event.
            myOven.BakeBread("Sourdough");

            // Trigger the event again with a different preheat time
            myOven.ConfigurePreheatTemperature(OvenTemperature.High);
            myOven.BakeBread("Wheat");
        }

        async private void BreadCompletedHandler1(Oven oven, Bread bread)
        {
            Action updateOutputText = () =>
                {
                    OvenClientOutput.Text += "Event Handler 1: Invoked\n";
                    OvenClientOutput.Text += "Event Handler 1: Oven volume is: " + oven.Volume.ToString() + "\n";
                    OvenClientOutput.Text += "Event Handler 1: Bread flavor is: " + bread.Flavor + "\n";
                };

            if (OvenClientOutput.Dispatcher.HasThreadAccess)
            {
                // If the current thread is the UI thread then execute the lambda.
                updateOutputText();
            }
            else
            {
                // If the current thread is not the UI thread use the dispatcher to execute the lambda on the UI thread.
                await OvenClientOutput.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, new Windows.UI.Core.DispatchedHandler(updateOutputText));
            }
        }

        async private void BreadCompletedHandler2(Oven oven, Bread bread)
        {
            Action updateOutputText = () =>
                {
                    OvenClientOutput.Text += "Event Handler 2: Invoked\n";
                };

            if (OvenClientOutput.Dispatcher.HasThreadAccess)
            {
                // If the current thread is the UI thread then execute the lambda.
                updateOutputText();
            }
            else
            {
                // If the current thread is not the UI thread use the dispatcher to execute the lambda on the UI thread.
                await OvenClientOutput.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, new Windows.UI.Core.DispatchedHandler(updateOutputText));
            }
        }

        async private void BreadCompletedHandler3(Oven oven, Bread bread)
        {
            // Event handler 3 was removed and will not be invoked
            Action updateOutputText = () =>
                {
                    OvenClientOutput.Text += "Event Handler 3: Invoked\n";
                };

            if (OvenClientOutput.Dispatcher.HasThreadAccess)
            {
                // If the current thread is the UI thread then execute the lambda.
                updateOutputText();
            }
            else
            {
                // If the current thread is not the UI thread use the dispatcher to execute the lambda on the UI thread.
                await OvenClientOutput.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, new Windows.UI.Core.DispatchedHandler(updateOutputText));
            }
        }
    }
}