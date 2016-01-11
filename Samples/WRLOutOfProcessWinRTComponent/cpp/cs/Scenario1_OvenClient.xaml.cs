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

namespace WRLOutOfProcessWinRTComponent
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class OvenClient : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        // Keep a ref to the oven since it is an event source.
        Oven _myOven = null;

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
        /// Use the custom Oven and BreadBakedEventArgs Windows Runtime components. 
        /// </summary>
        /// <param name="sender">Contains information about the button that fired the event.</param>
        /// <param name="e">Contains state information and event data associated with a routed event.</param>
        private void Start_Click(object sender, RoutedEventArgs e)
        {
            // Component Creation
            if (_myOven == null)
            {
                Dimensions dimensions;
                dimensions.Width = 2;
                dimensions.Height = 2;
                dimensions.Depth = 2;

                _myOven = new Oven(dimensions);
            }

            // Getters and setters are accessed using property syntax
            OvenClientOutput.Text += "Oven volume is: " + _myOven.Volume.ToString() + "\n";

            // Register even listeners
            _myOven.BreadBaked += new TypedEventHandler<Oven, BreadBakedEventArgs>(BreadCompletedHandler1);
            _myOven.BreadBaked += new TypedEventHandler<Oven, BreadBakedEventArgs>(BreadCompletedHandler2);
            _myOven.BreadBaked += new TypedEventHandler<Oven, BreadBakedEventArgs>(BreadCompletedHandler3);

            // Unregister from an event using the -= syntax and a delegate instance
            _myOven.BreadBaked -= new TypedEventHandler<Oven, BreadBakedEventArgs>(BreadCompletedHandler3);

            // Bake a loaf of bread. This will trigger the BreadBaked event.
            _myOven.BakeBread("Sourdough");

            // Trigger the event again with a different preheat time
            _myOven.ConfigurePreheatTemperature(OvenTemperature.High);
            _myOven.BakeBread("Wheat");
        }

        async private void BreadCompletedHandler1(Oven oven, BreadBakedEventArgs args)
        {
            Action updateOutputText = () =>
                {
                    OvenClientOutput.Text += "Event Handler 1: Invoked\n";
                    OvenClientOutput.Text += "Event Handler 1: Oven volume is: " + oven.Volume.ToString() + "\n";
                    OvenClientOutput.Text += "Event Handler 1: Bread flavor is: " + args.Bread.Flavor + "\n";
                };

            await OvenClientOutput.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, new Windows.UI.Core.DispatchedHandler(updateOutputText));
        }

        async private void BreadCompletedHandler2(Oven oven, BreadBakedEventArgs bread)
        {
            Action updateOutputText = () =>
                {
                    OvenClientOutput.Text += "Event Handler 2: Invoked\n";
                };
            await OvenClientOutput.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, new Windows.UI.Core.DispatchedHandler(updateOutputText));
        }

        async private void BreadCompletedHandler3(Oven oven, BreadBakedEventArgs bread)
        {
            // Event handler 3 was removed and will not be invoked
            Action updateOutputText = () =>
                {
                    OvenClientOutput.Text += "Event Handler 3: Invoked\n";
                };

            await OvenClientOutput.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, new Windows.UI.Core.DispatchedHandler(updateOutputText));
        }
    }
}
