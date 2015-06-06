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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using Windows.Devices.Sensors;
using Windows.Foundation;
using System.Threading.Tasks;
using Windows.UI.Core;

namespace SimpleOrientationCS
{
    public sealed partial class Scenario1_DataEvents : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private SimpleOrientationSensor _sensor;

        public Scenario1_DataEvents()
        {
            this.InitializeComponent();

            _sensor = SimpleOrientationSensor.GetDefault();
            if (_sensor == null)
            {
                rootPage.NotifyUser("No simple orientation sensor found", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached. The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }

        /// <summary>
        /// Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
        /// </summary>
        /// <param name="e">
        /// Event data that can be examined by overriding code. The event data is representative
        /// of the navigation that will unload the current Page unless canceled. The
        /// navigation can potentially be canceled by setting Cancel.
        /// </param>
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            if (ScenarioDisableButton.IsEnabled)
            {
                Window.Current.VisibilityChanged -= new WindowVisibilityChangedEventHandler(VisibilityChanged);
                _sensor.OrientationChanged -= new TypedEventHandler<SimpleOrientationSensor, SimpleOrientationSensorOrientationChangedEventArgs>(OrientationChanged);
            }

            base.OnNavigatingFrom(e);
        }

        /// <summary>
        /// This is the event handler for VisibilityChanged events. You would register for these notifications
        /// if handling sensor data when the app is not visible could cause unintended actions in the app.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e">
        /// Event data that can be examined for the current visibility state.
        /// </param>
        private void VisibilityChanged(object sender, VisibilityChangedEventArgs e)
        {
            if (ScenarioDisableButton.IsEnabled)
            {
                if (e.Visible)
                {
                    // Re-enable sensor input (no need to restore the desired reportInterval... it is restored for us upon app resume)
                    _sensor.OrientationChanged += new TypedEventHandler<SimpleOrientationSensor, SimpleOrientationSensorOrientationChangedEventArgs>(OrientationChanged);
                }
                else
                {
                    // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
                    _sensor.OrientationChanged -= new TypedEventHandler<SimpleOrientationSensor, SimpleOrientationSensorOrientationChangedEventArgs>(OrientationChanged);
                }
            }
        }

        /// <summary>
        /// Helper method to display the device orientation in the specified text box.
        /// </summary>
        /// <param name="tb">
        /// The text box receiving the orientation value.
        /// </param>
        /// <param name="orientation">
        /// The orientation value.
        /// </param>
        private void DisplayOrientation(TextBlock tb, SimpleOrientation orientation)
        {
            switch (orientation)
            {
                case SimpleOrientation.NotRotated:
                    tb.Text = "Not Rotated";
                    break;
                case SimpleOrientation.Rotated90DegreesCounterclockwise:
                    tb.Text = "Rotated 90 Degrees Counterclockwise";
                    break;
                case SimpleOrientation.Rotated180DegreesCounterclockwise:
                    tb.Text = "Rotated 180 Degrees Counterclockwise";
                    break;
                case SimpleOrientation.Rotated270DegreesCounterclockwise:
                    tb.Text = "Rotated 270 Degrees Counterclockwise";
                    break;
                case SimpleOrientation.Faceup:
                    tb.Text = "Faceup";
                    break;
                case SimpleOrientation.Facedown:
                    tb.Text = "Facedown";
                    break;
                default:
                    tb.Text = "Unknown orientation";
                    break;
            }
        }

        /// <summary>
        /// This is the event handler for OrientationChanged events.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void OrientationChanged(object sender, SimpleOrientationSensorOrientationChangedEventArgs e)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                DisplayOrientation(ScenarioOutput_Orientation, e.Orientation);
            });
        }

        /// <summary>
        /// This is the click handler for the 'Enable' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioEnable(object sender, RoutedEventArgs e)
        {
            if (_sensor != null)
            {
                Window.Current.VisibilityChanged += new WindowVisibilityChangedEventHandler(VisibilityChanged);
                _sensor.OrientationChanged += new TypedEventHandler<SimpleOrientationSensor, SimpleOrientationSensorOrientationChangedEventArgs>(OrientationChanged);

                ScenarioEnableButton.IsEnabled = false;
                ScenarioDisableButton.IsEnabled = true;

                // Display the current orientation once while waiting for the next orientation change
                DisplayOrientation(ScenarioOutput_Orientation, _sensor.GetCurrentOrientation());
            }
            else
            {
                rootPage.NotifyUser("No simple orientation sensor found", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for the 'Disable' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioDisable(object sender, RoutedEventArgs e)
        {
            Window.Current.VisibilityChanged -= new WindowVisibilityChangedEventHandler(VisibilityChanged);
            _sensor.OrientationChanged -= new TypedEventHandler<SimpleOrientationSensor, SimpleOrientationSensorOrientationChangedEventArgs>(OrientationChanged);

            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }
    }
}
