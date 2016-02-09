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
        /// Private MainPage object for status updates
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
        /// Initialize the lamp acquiring the default instance
        /// </summary>
        /// <returns>async Task</returns>
        private async Task InitializeLampAsync()
        {
            lamp = await Lamp.GetDefaultAsync();

            if (lamp == null)
            {
                await LogStatusAsync("Error: No lamp device was found", NotifyType.ErrorMessage);
                lampToggle.IsEnabled = false;
                return;
            }

            await LogStatusAsync(string.Format(CultureInfo.InvariantCulture, "Default lamp instance acquired, Device Id: {0}", lamp.DeviceId), NotifyType.StatusMessage);
            lampToggle.IsEnabled = true;
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
            if (lamp == null)
            {
                return;
            }

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
        private async void RegisterBtn_Click(object sender, RoutedEventArgs e)
        {
            if (lamp == null)
            {
                await LogStatusAsync("Error: No lamp device was found", NotifyType.ErrorMessage);
                return;
            }

            lamp.AvailabilityChanged += Lamp_AvailabilityChanged;
            await LogStatusAsync("Registered for Lamp Availability Changed Notification", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Event Handler for Lamp Availability Changed. When it fires, we want to update to
        /// Lamp toggle UI to show that lamp is available or not
        /// </summary>
        /// <param name="sender">Contains information regarding  Lamp object that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private async void Lamp_AvailabilityChanged(Lamp sender, LampAvailabilityChangedEventArgs args)
        {
            // Update the Lamp Toggle UI to indicate lamp has been consumed by another application
            await this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                if (lampToggle.IsOn)
                {
                    lampToggle.IsOn = args.IsAvailable;
                }

                lampToggle.IsEnabled = args.IsAvailable;
            });

            await LogStatusAsync(string.Format(CultureInfo.InvariantCulture,"Lamp Availability Changed Notification has fired, IsAvailable= {0}", args.IsAvailable), NotifyType.StatusMessage);
        }

        /// <summary>
        /// Event handler for the Unregister Availability Event
        /// </summary>
        /// <param name="sender">Contains information regarding button that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private async void UnRegisterBtn_Click(object sender, RoutedEventArgs e)
        {
            if (lamp == null)
            {
                await LogStatusAsync("Error: No lamp device was found", NotifyType.ErrorMessage);
                return;
            }

            lamp.AvailabilityChanged -= Lamp_AvailabilityChanged;
            await LogStatusAsync("Unregistered for Lamp Availability Changed Notification", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Event handler for Lamp Toggle Switch
        /// Toggling the switch will turn Lamp on or off
        /// </summary>
        /// <param name="sender">Contains information regarding toggle switch that fired event</param>
        /// <param name="e">Contains state information and event data associated with the event</param>
        private async void lampToggle_Toggled(object sender, RoutedEventArgs e)
        {
            if (lamp == null)
            {
                await LogStatusAsync("Error: No lamp device was found", NotifyType.ErrorMessage);
                return;
            }

            ToggleSwitch toggleSwitch = sender as ToggleSwitch;
            if (toggleSwitch != null)
            {
                if (toggleSwitch.IsOn)
                {
                    lamp.IsEnabled = true;
                    await LogStatusAsync("Lamp is Enabled", NotifyType.StatusMessage);
                }
                else
                {
                    lamp.IsEnabled = false;
                    await LogStatusAsync("Lamp is Disabled", NotifyType.StatusMessage);
                }
            }
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
        /// this method is for important status messages
        /// </summary>
        /// <param name="message">Message to log</param>
        /// <param name="type">Status notification type</param>
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
