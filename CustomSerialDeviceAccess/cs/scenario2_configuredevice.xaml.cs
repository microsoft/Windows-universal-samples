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
    public sealed partial class Scenario2_ConfigureDevice : Page
    {

        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;
        
        public Scenario2_ConfigureDevice()
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
                ConfigureDeviceScrollViewer.Visibility = Visibility.Collapsed;
                MainPage.Current.NotifyUser("Device is not connected", NotifyType.ErrorMessage);
            }
            else
            {
                if (EventHandlerForDevice.Current.Device.PortName != "")
                {
                    MainPage.Current.NotifyUser("Connected to " + EventHandlerForDevice.Current.Device.PortName,
                                                NotifyType.StatusMessage);
                }
                                            
                UpdateBaudRateView();
                UpdateParityView();
                UpdateHandShakeView();
                UpdateBreakStateSignalView();
                UpdateCarrierDetectStateView();
                UpdateDataBitsView();
                UpdateDataSetReadyStateView();
                UpdateDataTerminalReadyEnabledView();
                UpdateRequestToSendEnabledView();
                UpdateStopBitCountView();
            }
        }

        private void UpdateBaudRateView()
        {
            BaudRateValue.Text = EventHandlerForDevice.Current.Device.BaudRate.ToString();
        }

        private void BaudRateSetButton_Click(object sender, RoutedEventArgs e)
        {
            uint BaudRateInput = uint.Parse(BaudRateInputValue.Text);
            if (BaudRateInput != 0) 
            {
                EventHandlerForDevice.Current.Device.BaudRate = BaudRateInput;
            }
            UpdateBaudRateView();
        }
        
        private void UpdateParityView()
        {
            ParityValue.Text = EventHandlerForDevice.Current.Device.Parity.ToString();
            ParityComboBox.SelectedIndex = ParityComboBox.Items.IndexOf(ParityValue.Text);
        }
        
        private void ParityComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (ParityComboBox.SelectedItem.Equals("None"))
            {
                EventHandlerForDevice.Current.Device.Parity = SerialParity.None;
            }
            else if (ParityComboBox.SelectedItem.Equals("Even"))
            {
                EventHandlerForDevice.Current.Device.Parity = SerialParity.Even;
            }
            else if (ParityComboBox.SelectedItem.Equals("Odd"))
            {
                EventHandlerForDevice.Current.Device.Parity = SerialParity.Odd;
            }
            else if (ParityComboBox.SelectedItem.Equals("Mark"))
            {
                EventHandlerForDevice.Current.Device.Parity = SerialParity.Mark;
            }
            else if (ParityComboBox.SelectedItem.Equals("Space"))
            {
                EventHandlerForDevice.Current.Device.Parity = SerialParity.Space;
            }
            UpdateParityView();
        }

        private void UpdateStopBitCountView()
        {
            StopBitCountValue.Text = EventHandlerForDevice.Current.Device.StopBits.ToString();
            if (StopBitCountValue.Text.Equals("None") == false)
            {
                StopBitCountComboBox.SelectedIndex = StopBitCountComboBox.Items.IndexOf(StopBitCountValue.Text);
            }
        }
        
        private void StopBitCountComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (StopBitCountComboBox.SelectedItem.Equals("One"))
            {
                EventHandlerForDevice.Current.Device.StopBits = SerialStopBitCount.One;
            }
            else if (StopBitCountComboBox.SelectedItem.Equals("OnePointFive"))
            {
                EventHandlerForDevice.Current.Device.StopBits = SerialStopBitCount.OnePointFive;
            }
            else if (StopBitCountComboBox.SelectedItem.Equals("Two"))
            {
                EventHandlerForDevice.Current.Device.StopBits = SerialStopBitCount.Two;
            }
            UpdateStopBitCountView();
        }

        private void UpdateHandShakeView()
        {
            HandShakeValue.Text = EventHandlerForDevice.Current.Device.Handshake.ToString();
            HandShakeComboBox.SelectedIndex = HandShakeComboBox.Items.IndexOf(HandShakeValue.Text);
        }
        
        private void HandShakeComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (HandShakeComboBox.SelectedItem.Equals("None"))
            {
                EventHandlerForDevice.Current.Device.Handshake = SerialHandshake.None;
            }
            else if (HandShakeComboBox.SelectedItem.Equals("RequestToSend"))
            {
                EventHandlerForDevice.Current.Device.Handshake = SerialHandshake.RequestToSend;
            }
            else if (HandShakeComboBox.SelectedItem.Equals("XOnXOff"))
            {
                EventHandlerForDevice.Current.Device.Handshake = SerialHandshake.RequestToSendXOnXOff;
            }
            else if (HandShakeComboBox.SelectedItem.Equals("RequestToSendXOnXOff"))
            {
                EventHandlerForDevice.Current.Device.Handshake = SerialHandshake.RequestToSendXOnXOff;
            }
            UpdateHandShakeView();
        }

        private void UpdateBreakStateSignalView()
        {
            bool currentState = EventHandlerForDevice.Current.Device.BreakSignalState;
            if (currentState)
            {
                BreakStateSignalValue.Text = " On";
            }
            else
            {
                BreakStateSignalValue.Text = " Off";
            }
            BreakStateSignalToggleSwitch.IsOn = currentState;
        }
        
        private void BreakStateSignalToggleSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            EventHandlerForDevice.Current.Device.BreakSignalState = BreakStateSignalToggleSwitch.IsOn;
            UpdateBreakStateSignalView();
        }

        private void UpdateDataTerminalReadyEnabledView()
        {
            bool currentState = EventHandlerForDevice.Current.Device.IsDataTerminalReadyEnabled;
            if (currentState)
            {
                DataTerminalReadyEnabledValue.Text = "On";
            }
            else
            {
                DataTerminalReadyEnabledValue.Text = "Off";
            }
            DataTerminalReadyEnabledToggleSwitch.IsOn = currentState;
        }
        
        private void DataTerminalReadyEnabledToggleSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            EventHandlerForDevice.Current.Device.IsDataTerminalReadyEnabled = DataTerminalReadyEnabledToggleSwitch.IsOn;
            UpdateDataTerminalReadyEnabledView();
        }

        private void UpdateRequestToSendEnabledView()
        {
            bool currentState = EventHandlerForDevice.Current.Device.IsRequestToSendEnabled;
            if (currentState)
            {
                RequestToSendEnabledValue.Text = "On";
            }
            else
            {
                RequestToSendEnabledValue.Text = "Off";
            }
            RequestToSendEnabledToggleSwitch.IsOn = currentState;
        }
        
        private void RequestToSendEnabledToggleSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            EventHandlerForDevice.Current.Device.IsRequestToSendEnabled = RequestToSendEnabledToggleSwitch.IsOn;
            UpdateRequestToSendEnabledView();
        }

        private void UpdateCarrierDetectStateView()
        {
            if (EventHandlerForDevice.Current.Device.CarrierDetectState)
            {
                CarrierDetectStateValue.Text = "On";
            }
            else
            {
                CarrierDetectStateValue.Text = "Off";
            }
        }

        private void UpdateDataSetReadyStateView()
        {
            if (EventHandlerForDevice.Current.Device.DataSetReadyState)
            {
                DataSetReadyStateValue.Text = "On";
            }
            else
            {
                DataSetReadyStateValue.Text = "Off";
            }
        }

        private void UpdateDataBitsView()
        {
            ushort dataBits = EventHandlerForDevice.Current.Device.DataBits;
            DataBitsValue.Text = dataBits.ToString();
            if (dataBits <= 8)
            {
                DataBitsComboBox.SelectedIndex = dataBits;
            }
        }
        
        private void DataBitsComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            EventHandlerForDevice.Current.Device.DataBits = ushort.Parse(DataBitsComboBox.SelectedValue.ToString());
            UpdateDataBitsView();
        }

    }
}
