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
        /// Private MainPage object for status updates
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
            await LogStatusToOutputBoxAsync("Initializing lamp");

            // acquiring lamp instance via using statement allows for the 
            // object to be automatically disposed once the lamp object
            // goes out of scope releasing native resources
            using (var lamp = await Lamp.GetDefaultAsync())
            {
                if (lamp == null)
                {
                    await LogStatusAsync("Error: No lamp device was found", NotifyType.ErrorMessage);
                    return;
                }

                await LogStatusAsync(string.Format(CultureInfo.InvariantCulture, "Default lamp instance acquired, Device Id: {0}", lamp.DeviceId), NotifyType.StatusMessage);
                await LogStatusToOutputBoxAsync("Lamp Default settings:");
                await LogStatusToOutputBoxAsync(string.Format(CultureInfo.InvariantCulture, "Lamp Enabled: {0}, Brightness: {1}", lamp.IsEnabled, lamp.BrightnessLevel));

                // Set the Brightness Level
                await LogStatusToOutputBoxAsync("Adjusting Brightness");
                lamp.BrightnessLevel = 0.5F;
                await LogStatusAsync(string.Format(CultureInfo.InvariantCulture, "Lamp Settings After Brightness Adjustment: Brightness: {0}", lamp.BrightnessLevel), NotifyType.StatusMessage);

                // Turn Lamp on
                await LogStatusToOutputBoxAsync("Turning Lamp on");
                lamp.IsEnabled = true;
                await LogStatusToOutputBoxAsync(string.Format(CultureInfo.InvariantCulture, "Lamp Enabled: {0}", lamp.IsEnabled));

                // Turn Lamp off
                await LogStatusToOutputBoxAsync("Turning Lamp off");
                lamp.IsEnabled = false;
                await LogStatusToOutputBoxAsync(string.Format(CultureInfo.InvariantCulture, "Lamp Enabled: {0}", lamp.IsEnabled));

            }

            await LogStatusToOutputBoxAsync("Lamp Disposed");
        }

        /// <summary>
        /// Event handler for Color Lamp Button. Queries the lamp device for color adjustment
        /// support and if device supports being color settable, it sets lamp color to blue. 
        /// Click Event
        /// </summary>
        /// <param name="sender">Contains information regarding button that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private async void ColorLampBtn_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            await LogStatusToOutputBoxAsync("Initializing lamp");

            // Acquiring lamp instance via using statement allows for the 
            // object to be automatically disposed once the lamp object
            // goes out of scope releasing native resources
            using (var lamp = await Lamp.GetDefaultAsync())
            {
                if (lamp == null)
                {
                    await LogStatusAsync("Error: No lamp device was found", NotifyType.ErrorMessage);
                    return;
                }

                // With Lamp instance required, query for the default settings of the Lamp
                await LogStatusAsync("Default lamp instance acquired", NotifyType.StatusMessage);
                await LogStatusToOutputBoxAsync("Lamp Default settings:");
                await LogStatusToOutputBoxAsync(string.Format(CultureInfo.InvariantCulture, "Lamp Enabled: {0}, Is Color Settable: {1}, Color: {2}", lamp.IsEnabled, lamp.IsColorSettable, lamp.Color));

                // If this lamp device is not Color Settable exit color lamp adjustment
                if (lamp.IsColorSettable == false)
                {
                    await LogStatusAsync("Selected Lamp device doesn't support Color lamp adjustment", NotifyType.ErrorMessage);
                    return;
                }

                // Change Lamp Color
                await LogStatusToOutputBoxAsync("Adjusting Color");
                lamp.Color = Colors.Blue;
                await LogStatusAsync(string.Format(CultureInfo.InvariantCulture, "Lamp Settings After Color Adjustment: Color: {0}", lamp.Color), NotifyType.StatusMessage);

                // Turn Lamp on
                await LogStatusToOutputBoxAsync("Turning Lamp on");
                lamp.IsEnabled = true;
                await LogStatusToOutputBoxAsync(string.Format(CultureInfo.InvariantCulture, "Lamp Enabled: {0}", lamp.IsEnabled));

                // Turn Lamp off
                await LogStatusToOutputBoxAsync("Turning Lamp off");
                lamp.IsEnabled = false;
                await LogStatusToOutputBoxAsync(string.Format(CultureInfo.InvariantCulture, "Lamp Enabled: {0}", lamp.IsEnabled));
            }

            /// Lamp gets disposed automatically when it goes out of scope of using block
            await LogStatusToOutputBoxAsync("Lamp Disposed");
        }

        /// <summary>
        /// Marshall log message to the UI thread
        /// and update outputBox, this method is for more general messages
        /// </summary>
        /// <param name="message">Message to log</param>
        private async Task LogStatusToOutputBoxAsync(string message)
        {
            await this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                outputBox.Text += message + "\r\n";
                outputScrollViewer.ChangeView(0, outputBox.ActualHeight, 1);
            });
        }

        /// <summary>
        /// Method to log Status to Main Status block,
        /// which shows up in bottom of scenario page status block
        /// </summary>
        /// <param name="message">Message to log</param>
        /// <param name="type">Status Notify Type</param>
        private async Task LogStatusAsync(string message, NotifyType type)
        {
            await this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser(message, type);
            });
            await LogStatusToOutputBoxAsync(message);
        }
    }
}
