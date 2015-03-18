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
    using SDKTemplate;
    using System;
    using System.Globalization;
    using System.Linq;
    using Windows.Devices.Lights;
    using Windows.Devices.Enumeration;
    using Windows.UI.Xaml;
    using Windows.UI.Xaml.Controls;
    using Windows.UI.Xaml.Navigation;

    /// <summary>
    /// Scenario 1 Get Lamp Page
    /// </summary>
    public sealed partial class Scenario1_GetLamp : Page
    {
        /// <summary>
        /// Private Mainpage object for status updates
        /// </summary>
        private MainPage rootPage;

        /// <summary>
        /// Initializes a new instance of the <see cref="Scenario1_GetLamp"/> class.
        /// </summary>
        public Scenario1_GetLamp()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Override default OnNavigateTo to initialize 
        /// the rootPage object for Status message updates
        /// </summary>
        /// <param name="e"></param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            base.OnNavigatedTo(e);
        }

        /// <summary>
        /// acquires Lamp instance by getting class selection string 
        /// for Lamp devices, then selecting the first lamp device on
        /// the back of the device
        /// </summary>
        /// <param name="sender">Contains information regarding button that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private async void InitLampBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                LogStatusToOutputBox("Getting class selection string for lamp devices..");

                // Returns class selection string for Lamp devices
                string selectorStr = Lamp.GetDeviceSelector();
                LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Lamp class selection string:\n {0}", selectorStr));

                LogStatusToOutputBox("Finding all lamp devices...");

                // Finds all devices based on lamp class selector string
                DeviceInformationCollection devices = await DeviceInformation.FindAllAsync(selectorStr);
                LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Number of lamp devices found: {0}", devices.Count.ToString()));

                // Select the first lamp device found on the back of the device
                LogStatusToOutputBox("Selecting first lamp device found on back of the device");
                DeviceInformation deviceInfo = devices.FirstOrDefault(di => di.EnclosureLocation != null && di.EnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Back);

                if (deviceInfo == null)
                {
                    throw new InvalidOperationException("Error: No lamp device was found");
                }

                LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Acquiring Lamp instance from Id:\n {0}", deviceInfo.Id));
                Lamp lamp = await Lamp.FromIdAsync(deviceInfo.Id);
                LogStatus(string.Format(CultureInfo.InvariantCulture, "Lamp instance acquired, Device Id:\n {0}", lamp.DeviceId), NotifyType.StatusMessage);

                // Here we must Dispose of the lamp object once we are no longer 
                // using it to release any native resources
                LogStatusToOutputBox("Disposing of lamp instance");
                lamp.Dispose();
                lamp = null;
                LogStatusToOutputBox("Disposed");
            }
            catch(Exception eX)
            {
                LogStatus(eX.Message, NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Gets the Default Lamp instance, if no instance is found it throws
        /// InvalidOperation Exception
        /// </summary>
        /// <param name="sender">Contains information regarding button that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private async void GetDefaultAsyncBtn_Click(object sender, RoutedEventArgs e)
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

                        LogStatus(string.Format(CultureInfo.InvariantCulture, "Default lamp instance acquired, Device Id: {0}", lamp.DeviceId), NotifyType.StatusMessage);
                    }

                    // Lamp object is automatically Disposed now that it's out of scope
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
        /// <param name="message">Message to log to the outputBox</param>
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
        /// this method is for important status messages to be 
        /// displayed at bottom of scenario page
        /// </summary>
        /// <param name="message">Message to log to the output box</param>
        /// <param name="type">Notification Type for status message</param>
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
