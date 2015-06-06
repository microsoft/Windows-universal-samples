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
using Windows.Devices.SerialCommunication;
using SDKTemplate;
using CustomSerialDeviceAccess;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace CustomSerialDeviceAccess
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario4_Events : Page
    {

        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;
        public Scenario4_Events()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        ///
        /// We will enable/disable parts of the UI if the device doesn't support it.
        /// </summary>
        /// <param name="eventArgs">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs eventArgs)
        {

            if (EventHandlerForDevice.Current.Device == null)
            {
                EventsScrollViewer.Visibility = Visibility.Collapsed;
                MainPage.Current.NotifyUser("Device is not connected", NotifyType.ErrorMessage);
            }
            else
            {
                if (EventHandlerForDevice.Current.Device.PortName != "")
                {
                    MainPage.Current.NotifyUser("Connected to " + EventHandlerForDevice.Current.Device.PortName,
                                                NotifyType.StatusMessage);
                }
            }
        }

        protected override void OnNavigatedFrom(NavigationEventArgs eventArgs)
        {
            PinChangedToggleSwitch.IsOn = false;
            ErrorReceivedToggleSwitch.IsOn = false;
        }

        private Windows.Foundation.TypedEventHandler<SerialDevice, PinChangedEventArgs> PinChangedEventHandler;
        private Windows.Foundation.TypedEventHandler<SerialDevice, ErrorReceivedEventArgs> ErrorReceivedEventHandler;

        private void PinChangedEvent(SerialDevice sender, PinChangedEventArgs eventArgs)
        {
            rootPage.NotifyUser("Pin Changed Event Received", NotifyType.StatusMessage);
        }


        private void ErrorReceivedEvent(SerialDevice sender, ErrorReceivedEventArgs eventArgs)
        {
            rootPage.NotifyUser("Error Received Event Received", NotifyType.ErrorMessage);
        }

        private void PinChangedToggleSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            if (PinChangedToggleSwitch.IsOn)
            {
                PinChangedEventHandler = new Windows.Foundation.TypedEventHandler<SerialDevice, PinChangedEventArgs>(this.PinChangedEvent);
                rootPage.NotifyUser("Pin Changed Event registered", NotifyType.StatusMessage);
                EventHandlerForDevice.Current.Device.PinChanged += PinChangedEventHandler;
                PinChangedValue.Text = "On";
            }
            else
            {
                rootPage.NotifyUser("Pin Changed Event unregistered", NotifyType.StatusMessage);
                EventHandlerForDevice.Current.Device.PinChanged += PinChangedEventHandler;
                PinChangedValue.Text = "Off";
            }
        }

        private void ErrorReceivedToggleSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            if (ErrorReceivedToggleSwitch.IsOn)
            {
                ErrorReceivedEventHandler = new Windows.Foundation.TypedEventHandler<SerialDevice, ErrorReceivedEventArgs>(this.ErrorReceivedEvent);
                rootPage.NotifyUser("Error Received Event registered", NotifyType.StatusMessage);
                EventHandlerForDevice.Current.Device.ErrorReceived += ErrorReceivedEventHandler;
                ErrorReceivedValue.Text = "On";
            }
            else
            {
                rootPage.NotifyUser("Error Received Event unregistered", NotifyType.StatusMessage);
                EventHandlerForDevice.Current.Device.ErrorReceived -= ErrorReceivedEventHandler;
                ErrorReceivedValue.Text = "Off";
            }
        }
    }
}
