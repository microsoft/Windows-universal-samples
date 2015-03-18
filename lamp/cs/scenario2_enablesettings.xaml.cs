//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

namespace Lamp
{
    using Windows.UI;
    using Windows.UI.Xaml.Controls;
    using Windows.UI.Xaml.Navigation;
    using SDKTemplate;
    using System;
    using System.Globalization;
    using System.Threading.Tasks;
    using Windows.Devices.Lights;

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2_EnableSettings : Page
    {
        /// <summary>
        /// Private Mainpage object for status updates
        /// </summary>
        private MainPage rootPage;

        /// <summary>
        /// Initializes a new instance of the <see cref="Scenario2_EnableSettings"/> class.
        /// </summary>
        public Scenario2_EnableSettings()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Event Handler for OnNavigateTo Event
        /// </summary>
        /// <param name="e"></param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            base.OnNavigatedTo(e);
        }

        /// <summary>
        /// Event Handler for Brightness Button Click
        /// </summary>
        /// <param name="sender">Contains information regarding button that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private async void BrightnessBtn_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
                try
                {
                    LogStatusToOutputBox("Initializing lamp");

                    // acquiring lamp instance via using statement allows for the 
                    // object to be automatically disposed once the lamp object
                    // goes out of scope releasing native resources
                    using (var lamp = await Lamp.GetDefaultAsync())
                    {
                        if (lamp == null)
                        {
                            throw new InvalidOperationException("Error: Lamp could not be acquired");
                        }

                        LogStatus("Default lamp instance acquired", NotifyType.StatusMessage);
                        LogStatusToOutputBox("Lamp Default settings:");
                        LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Lamp Enabled: {0}, Brightness: {1}", lamp.IsEnabled, lamp.BrightnessLevel));

                        // Set the Brightness Level
                        LogStatusToOutputBox("Adjusting Brightness");
                        lamp.BrightnessLevel = 0.5F;
                        LogStatus(string.Format(CultureInfo.InvariantCulture, "Lamp Settings After Brightness Adjustment: Brightness: {0}", lamp.BrightnessLevel), NotifyType.StatusMessage);

                        // Turn Lamp on
                        LogStatusToOutputBox("Turning Lamp on");
                        lamp.IsEnabled = true;
                        LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Lamp Enabled: {0}", lamp.IsEnabled));

                        // Turn Lamp off
                        LogStatusToOutputBox("Turning Lamp off");
                        lamp.IsEnabled = false;
                        LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Lamp Enabled: {0}", lamp.IsEnabled));

                    }

                    LogStatusToOutputBox("Lamp Disposed");
                }
                catch (Exception ex)
                {
                    LogStatus(ex.ToString(), NotifyType.ErrorMessage);
                }
        }

        /// <summary>
        /// Event handler for Color Lamp Button
        /// Click Event
        /// </summary>
        /// <param name="sender">Contains information regarding button that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private async void ColorLampBtn_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
                try
                {
                    LogStatusToOutputBox("Initializing lamp");

                    // Acquiring lamp instance via using statement allows for the 
                    // object to be automatically disposed once the lamp object
                    // goes out of scope releasing native resources
                    using (var lamp = await Lamp.GetDefaultAsync())
                    {
                        if (lamp == null)
                        {
                            throw new InvalidOperationException("Error: Lamp could not be acquired");
                        }

                        // With Lamp instance required, query for the default settings of the Lamp
                        LogStatus("Default lamp instance acquired", NotifyType.StatusMessage);
                        LogStatusToOutputBox("Lamp Default settings:");
                        LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Lamp Enabled: {0}, Is Color Settable: {1}, Color: {2}", lamp.IsEnabled, lamp.IsColorSettable, lamp.Color));

                        // Change Lamp Color
                        LogStatusToOutputBox("Adjusting Color");
                        lamp.Color = Colors.Blue;
                        LogStatus(string.Format(CultureInfo.InvariantCulture, "Lamp Settings After Color Adjustment: Color: {0}", lamp.Color), NotifyType.StatusMessage);

                        // Turn Lamp on
                        LogStatusToOutputBox("Turning Lamp on");
                        lamp.IsEnabled = true;
                        LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Lamp Enabled: {0}", lamp.IsEnabled));

                        // Turn Lamp off
                        LogStatusToOutputBox("Turning Lamp off");
                        lamp.IsEnabled = false;
                        LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Lamp Enabled: {0}", lamp.IsEnabled));
                    }

                    /// Lamp gets disposed automatically when it goes out of scope of using block
                    LogStatusToOutputBox("Lamp Disposed");
                }
                catch (Exception ex)
                {
                    LogStatus(ex.ToString(), NotifyType.ErrorMessage);
                }
        }

        /// <summary>
        /// Marshall log message to the UI thread
        /// and update outputBox, this method is for more general messages
        /// </summary>
        /// <param name="message">Message to log</param>
        private async void LogStatusToOutputBox(string message)
        {
            await this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                outputBox.Text += message + "\r\n";
                outputScrollViewer.ChangeView(0, outputBox.ActualHeight, 1);
            });
        }

        /// <summary>
        /// Method to log Status to Main Status block,
        /// which shows up in bottom of scenerio page status block
        /// </summary>
        /// <param name="message">Message to log</param>
        /// <param name="type">Status Notify Type</param>
        private async void LogStatus(string message, NotifyType type)
        {
            await this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser(message, type);
            });
            LogStatusToOutputBox(message);
        }
    }
}
