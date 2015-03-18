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
    using Windows.UI.Xaml;
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
    public sealed partial class Scenario3_AvailabilityChanged : Page
    {
        /// <summary>
        /// Private Mainpage object for status updates
        /// </summary>
        private MainPage rootPage;

        /// <summary>
        /// Private lamp object for the page
        /// </summary>
        private Lamp lamp;

        /// <summary>
        /// Initializes a new instance of the <see cref="Scenario3_AvailabilityChanged("/> class.
        /// </summary>
        public Scenario3_AvailabilityChanged()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Event handler for the OnNavigatedTo event. In this scenario, we want
        /// to get a lamp object for the lifetime of the page, so when we first 
        /// navigate to the page, we Initialize a private lamp object
        /// </summary>
        /// <param name="e">Contains state information and event data associated with the event</param>
        protected async override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            await InitializeLampAsync();
            base.OnNavigatedTo(e);
        }

        /// <summary>
        /// Intialize the lamp acquiring the default instance
        /// </summary>
        /// <returns>async Task</returns>
        private async Task InitializeLampAsync()
        {
            try
            {
                lamp = await Lamp.GetDefaultAsync();

                if (lamp == null)
                {
                    throw new InvalidOperationException("Error: No lamp device was found");
                }

                LogStatus(string.Format(CultureInfo.InvariantCulture, "Default lamp instance acquired, Device Id: {0}", lamp.DeviceId) , NotifyType.StatusMessage);
                lampToggle.IsEnabled = true;
            }
            catch (Exception eX)
            {
                LogStatus(eX.Message, NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Event handler for the OnNavigatingFrom event. Here 
        /// we want to make sure to release the lamp before exiting this scenario page.
        /// </summary>
        /// <param name="e">Contains state information and event data associated with the event</param>
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            DisposeLamp();
            base.OnNavigatingFrom(e);
        }

        /// <summary>
        /// Method to dispose of current Lamp instance
        /// and release all resources
        /// </summary>
        private void DisposeLamp()
        {
            lampToggle.IsEnabled = false;
            lamp.AvailabilityChanged -= Lamp_AvailabilityChanged;
            lamp.IsEnabled = false;
            lamp.Dispose();
            lamp = null;
        }

        /// <summary>
        /// Event Handler for the register button click event
        /// </summary>
        /// <param name="sender">Contains information regarding button that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private void RegisterBtn_Click(object sender, RoutedEventArgs e)
        {
            lamp.AvailabilityChanged += Lamp_AvailabilityChanged;
            LogStatus("Registered for Lamp Availability Changed Notification", NotifyType.StatusMessage);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender">Contains information regarding  Lamp object that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private void Lamp_AvailabilityChanged(Lamp sender, LampAvailabilityChangedEventArgs args)
        {
            // Update the Lamp Toggle UI to indicate lamp has been consumed by another app
            lampToggle.IsEnabled = args.IsAvailable;
            LogStatus(string.Format(CultureInfo.InvariantCulture,"Lamp Availability Changed Notification has fired, IsAvailable= {0}", args.IsAvailable), NotifyType.StatusMessage);
        }

        /// <summary>
        /// Event handler for the Unregister Availablity Event
        /// </summary>
        /// <param name="sender">Contains information regarding button that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private void UnRegisterBtn_Click(object sender, RoutedEventArgs e)
        {
            lamp.AvailabilityChanged -= Lamp_AvailabilityChanged;
            LogStatus("Unregistered for Lamp Availability Changed Notification", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Event handler for Lamp Toggle Switch
        /// Toggling the switch will turn Lamp on or off
        /// </summary>
        /// <param name="sender">Contains information regarding toggle switch that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private void lampToggle_Toggled(object sender, RoutedEventArgs e)
        {
            ToggleSwitch toggleSwitch = sender as ToggleSwitch;
            if (toggleSwitch != null)
            {
                if (toggleSwitch.IsOn)
                {
                    lamp.IsEnabled = true;
                    LogStatus("Lamp is Enabled", NotifyType.StatusMessage);
                }
                else
                {
                    lamp.IsEnabled = false;
                    LogStatus("Lamp is Disabled", NotifyType.StatusMessage);
                }
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
        /// this method is for important status messages
        /// </summary>
        /// <param name="message">Message to log</param>
        /// <param name="type">Status notification type</param>
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
