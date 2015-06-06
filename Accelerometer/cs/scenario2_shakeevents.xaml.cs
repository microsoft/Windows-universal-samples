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

// Windows Store code

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using Windows.Devices.Sensors;
using Windows.Foundation;
using Windows.UI.Core;

namespace AccelerometerCS
{
    public sealed partial class Scenario2_ShakeEvents : Page
    {
        private Accelerometer _accelerometer;
        private ushort _shakeCount;

        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario2_ShakeEvents()
        {
            this.InitializeComponent();

            _accelerometer = Accelerometer.GetDefault();
            if (_accelerometer == null)
            {
                rootPage.NotifyUser("No accelerometer found", NotifyType.ErrorMessage);
            }
            _shakeCount = 0;
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
                _accelerometer.Shaken -= new TypedEventHandler<Accelerometer, AccelerometerShakenEventArgs>(Shaken);
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
                    // Re-enable sensor input
                    _accelerometer.Shaken += new TypedEventHandler<Accelerometer, AccelerometerShakenEventArgs>(Shaken);
                }
                else
                {
                    // Disable sensor input
                    _accelerometer.Shaken -= new TypedEventHandler<Accelerometer, AccelerometerShakenEventArgs>(Shaken);
                }
            }
        }

        /// <summary>
        /// This is the event handler for Shaken events.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void Shaken(object sender, AccelerometerShakenEventArgs e)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                _shakeCount++;
                ScenarioOutputText.Text = _shakeCount.ToString();
            });
        }

        /// <summary>
        /// This is the click handler for the 'Enable' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioEnable(object sender, RoutedEventArgs e)
        {
            if (_accelerometer != null)
            {
                Window.Current.VisibilityChanged += new WindowVisibilityChangedEventHandler(VisibilityChanged);
                _accelerometer.Shaken += new TypedEventHandler<Accelerometer, AccelerometerShakenEventArgs>(Shaken);
                ScenarioEnableButton.IsEnabled = false;
                ScenarioDisableButton.IsEnabled = true;
            }
            else
            {
                rootPage.NotifyUser("No accelerometer found", NotifyType.ErrorMessage);
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
            _accelerometer.Shaken -= new TypedEventHandler<Accelerometer, AccelerometerShakenEventArgs>(Shaken);
            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }
    }
}
