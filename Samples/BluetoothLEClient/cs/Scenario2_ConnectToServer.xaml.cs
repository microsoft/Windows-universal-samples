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

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Security.Cryptography;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    // This scenario connects to the device selected in the "Discover
    // GATT Servers" scenario and communicates with it.
    // Note that this scenario is rather artificial because it communicates
    // with an unknown service with unknown characteristics.
    // In practice, your app will be interested in a specific service with
    // a specific characteristic.
    public sealed partial class Scenario2_ConnectToServer : Page
    {
        private MainPage rootPage = MainPage.Current;

        private ObservableCollection<BluetoothLEAttributeDisplay> ServiceCollection = new ObservableCollection<BluetoothLEAttributeDisplay>();
        private ObservableCollection<BluetoothLEAttributeDisplay> CharacteristicCollection = new ObservableCollection<BluetoothLEAttributeDisplay>();

        private BluetoothLEDevice bluetoothLeDevice = null;
        private GattCharacteristic selectedCharacteristic;
        private bool isValueChangedHandlerRegistered = false;
        private GattPresentationFormat presentationFormat;

        public Scenario2_ConnectToServer()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (string.IsNullOrEmpty(rootPage.SelectedBleDeviceId))
            {
                ConnectButton.IsEnabled = false;
            }
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            ClearSelectedCharacteristic();
            ClearBluetoothLEDevice();
        }

        #region Enumerating services
        private void ClearBluetoothLEDevice()
        {
            bluetoothLeDevice?.Dispose();
            bluetoothLeDevice = null;
        }

        private async void ConnectButton_Click()
        {
            ConnectButton.IsEnabled = false;

            ClearBluetoothLEDevice();
            try
            {
                // BT_Code: BluetoothLEDevice.FromIdAsync must be called from a UI thread because it may prompt for consent.
                bluetoothLeDevice = await BluetoothLEDevice.FromIdAsync(rootPage.SelectedBleDeviceId);
            }
            catch (Exception ex) when ((uint)ex.HResult == 0x800710df)
            {
                // ERROR_DEVICE_NOT_AVAILABLE because the Bluetooth radio is not on.
            }

            if (bluetoothLeDevice != null)
            {
                // BT_Code: GattServices returns a list of all the supported services of the device.
                // If the services supported by the device are expected to change
                // during BT usage, subscribe to the GattServicesChanged event.
                foreach (var service in bluetoothLeDevice.GattServices)
                {
                    ServiceCollection.Add(new BluetoothLEAttributeDisplay(service));
                }
                ConnectButton.Visibility = Visibility.Collapsed;
                ServiceList.Visibility = Visibility.Visible;
            }
            else
            {
                ClearBluetoothLEDevice();
                rootPage.NotifyUser("Failed to connect to device.", NotifyType.ErrorMessage);
            }
            ConnectButton.IsEnabled = true;
        }
        #endregion

        #region Enumerating characteristics
        private void ServiceList_SelectionChanged()
        {
            var attributeInfoDisp = (BluetoothLEAttributeDisplay)ServiceList.SelectedItem;

            CharacteristicCollection.Clear();
            IReadOnlyList<GattCharacteristic> characteristics = null;
            try
            {
                // BT_Code: Get all the child characteristics of a service.
                characteristics = attributeInfoDisp.service.GetAllCharacteristics();
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Restricted service. Can't read characteristics: " + ex.Message,
                    NotifyType.ErrorMessage);
                // On error, act as if there are no characteristics.
                characteristics = new List<GattCharacteristic>();
            }

            foreach (GattCharacteristic c in characteristics)
            {
                CharacteristicCollection.Add(new BluetoothLEAttributeDisplay(c));
            }
            CharacteristicList.Visibility = Visibility.Visible;
        }
        #endregion

        #region Accessing characteristics
        private void ClearSelectedCharacteristic()
        {
            if (selectedCharacteristic != null)
            {
                if (isValueChangedHandlerRegistered)
                {
                    selectedCharacteristic.ValueChanged -= Characteristic_ValueChanged;
                    isValueChangedHandlerRegistered = false;
                }
                selectedCharacteristic = null;
            }
        }

        private void AddValueChangedHandler()
        {
            if (!isValueChangedHandlerRegistered)
            {
                selectedCharacteristic.ValueChanged += Characteristic_ValueChanged;
                isValueChangedHandlerRegistered = true;
            }
        }

        private void CharacteristicList_SelectionChanged()
        {
            ClearSelectedCharacteristic();

            var attributeInfoDisp = (BluetoothLEAttributeDisplay)CharacteristicList.SelectedItem;
            if (attributeInfoDisp == null)
            {
                EnableCharacteristicPanels(GattCharacteristicProperties.None);
                return;
            }

            selectedCharacteristic = attributeInfoDisp.characteristic;
            isValueChangedHandlerRegistered = false;

            // BT_Code: There's no need to get presentation format unless there's at least one descriptor.
            presentationFormat = null;
            var descriptors = selectedCharacteristic.GetAllDescriptors();
            if (descriptors.Count > 0)
            {
                if (selectedCharacteristic.PresentationFormats.Count > 1)
                {
                    // It's difficult to figure out how to split up a characteristic and encode its different parts propertly.
                    // In this case, we'll just encode the whole thing to a string to make it easy to print out.
                }
                else if (selectedCharacteristic.PresentationFormats.Count == 1)
                {
                    // Get the presentation format since there's only one way of presenting it
                    presentationFormat = selectedCharacteristic.PresentationFormats[0];
                }
            }

            // Enable/disable operations based on the GattCharacteristicProperties.
            EnableCharacteristicPanels(selectedCharacteristic.CharacteristicProperties);
        }

        private void SetVisibility(UIElement element, bool visible)
        {
            element.Visibility = visible ? Visibility.Visible : Visibility.Collapsed;
        }

        private void EnableCharacteristicPanels(GattCharacteristicProperties properties)
        {
            // BT_Code: Hide the controls which do not apply to this characteristic.
            SetVisibility(CharacteristicReadPanel, properties.HasFlag(GattCharacteristicProperties.Read));

            SetVisibility(CharacteristicWritePanel,
                properties.HasFlag(GattCharacteristicProperties.Write) ||
                properties.HasFlag(GattCharacteristicProperties.WriteWithoutResponse));
            CharacteristicWriteValue.Text = "";

            SetVisibility(CharacteristicIndicatePanel, properties.HasFlag(GattCharacteristicProperties.Indicate));
            SetVisibility(CharacteristicNotifyPanel, properties.HasFlag(GattCharacteristicProperties.Notify));

            CharacteristicLatestValue.Text = "";
        }

        private async void CharacteristicReadButton_Click()
        {
            // BT_Code: Read the actual value from the device by using Uncached.
            GattReadResult result = await selectedCharacteristic.ReadValueAsync(BluetoothCacheMode.Uncached);
            if (result.Status == GattCommunicationStatus.Success)
            {
                string formattedResult = FormatValueByPresentation(result.Value, presentationFormat);
                rootPage.NotifyUser($"Read result: {formattedResult}", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser($"Read failed: {result.Status}", NotifyType.ErrorMessage);
            }
        }

        private async void CharacteristicWriteButton_Click()
        {
            if (!String.IsNullOrEmpty(CharacteristicWriteValue.Text))
            {
                var writeBuffer = CryptographicBuffer.ConvertStringToBinary(CharacteristicWriteValue.Text,
                    BinaryStringEncoding.Utf8);
                try
                {
                    // BT_Code: Writes the value from the buffer to the characteristic.
                    var result = await selectedCharacteristic.WriteValueAsync(writeBuffer);

                    if (result == GattCommunicationStatus.Success)
                    {
                        rootPage.NotifyUser("Successfully wrote value to device", NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser($"Write failed: {result}", NotifyType.ErrorMessage);
                    }
                }
                catch (Exception ex) when ((uint)ex.HResult == 0x80650003 || (uint)ex.HResult == 0x80070005)
                {
                    // E_BLUETOOTH_ATT_WRITE_NOT_PERMITTED or E_ACCESSDENIED
                    // This usually happens when a device reports that it support writing, but it actually doesn't.
                    rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("No data to write to device", NotifyType.ErrorMessage);
            }
        }

        private async void CharacteristicIndicateButton_Click()
        {
            try
            {
                // BT_Code: Must write the CCCD in order for server to send indications.
                // We receive them in the ValueChanged event handler.
                // Note that this sample configures either Indicate or Notify, but not both.
                var result = await
                        selectedCharacteristic.WriteClientCharacteristicConfigurationDescriptorAsync(
                            GattClientCharacteristicConfigurationDescriptorValue.Indicate);
                if (result == GattCommunicationStatus.Success)
                {
                    AddValueChangedHandler();
                    rootPage.NotifyUser("Successfully registered for indications", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser($"Error registering for indications: {result}", NotifyType.ErrorMessage);
                }
            }
            catch (UnauthorizedAccessException ex)
            {
                // This usually happens when a device reports that it support indicate, but it actually doesn't.
                rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
            }
        }

        private async void CharacteristicNotifyButton_Click()
        {
            try
            {
                // BT_Code: Must write the CCCD in order for server to send notifications.
                // We receive them in the ValueChanged event handler.
                // Note that this sample configures either Indicate or Notify, but not both.
                var result = await
                        selectedCharacteristic.WriteClientCharacteristicConfigurationDescriptorAsync(
                            GattClientCharacteristicConfigurationDescriptorValue.Notify);
                if (result == GattCommunicationStatus.Success)
                {
                    AddValueChangedHandler();
                    rootPage.NotifyUser("Successfully registered for notifications", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser($"Error registering for notifications: {result}", NotifyType.ErrorMessage);
                }
            }
            catch (UnauthorizedAccessException ex)
            {
                // This usually happens when a device reports that it support notify, but it actually doesn't.
                rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
            }
        }

        private async void Characteristic_ValueChanged(GattCharacteristic sender, GattValueChangedEventArgs args)
        {
            // BT_Code: An Indicate or Notify reported that the value has changed.
            // Display the new value with a timestamp.
            var newValue = FormatValueByPresentation(args.CharacteristicValue, presentationFormat);
            var message = $"Value at {DateTime.Now:hh:mm:ss.FFF}: {newValue}";
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
                () => CharacteristicLatestValue.Text = message);
        }
        #endregion

        private string FormatValueByPresentation(IBuffer buffer, GattPresentationFormat format)
        {
            // BT_Code: For the purpose of this sample, this function converts only UInt32 and
            // UTF-8 buffers to readable text. It can be extended to support other formats if your app needs them.
            byte[] data;
            CryptographicBuffer.CopyToByteArray(buffer, out data);
            if (format != null)
            {
                if (format.FormatType == GattPresentationFormatTypes.UInt32 && data.Length >= 4)
                {
                    return BitConverter.ToInt32(data, 0).ToString();
                }
                else if (format.FormatType == GattPresentationFormatTypes.Utf8)
                {
                    try
                    {
                        return Encoding.UTF8.GetString(data);
                    }
                    catch (ArgumentException)
                    {
                        return "(error: Invalid UTF-8 string)";
                    }
                }
                else
                {
                    // Add support for other format types as needed.
                    return "Unsupported format: " + CryptographicBuffer.EncodeToHexString(buffer);
                }
            }
            else
            {
                // We don't know what format to use. Let's try a well-known profile, or default back to UTF-8.
                if (selectedCharacteristic.Uuid.Equals(GattCharacteristicUuids.HeartRateMeasurement))
                {
                    try
                    {
                        return "Heart Rate: " + ParseHeartRateValue(data).ToString();
                    }
                    catch (ArgumentException)
                    {
                        return "Heart Rate: (unable to parse)";
                    }
                }
                else if (selectedCharacteristic.Uuid.Equals(GattCharacteristicUuids.BatteryLevel))
                {
                    try
                    {
                        // battery level is encoded as a percentage value in the first byte according to
                        // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.battery_level.xml
                        return "Battery Level: " + data[0].ToString() + "%";
                    }
                    catch (ArgumentException)
                    {
                        return "Battery Level: (unable to parse)";
                    }
                }
                else
                {
                    try
                    {
                        return "Unknown format: " + Encoding.UTF8.GetString(data);
                    }
                    catch (ArgumentException)
                    {
                        return "Unknown format";
                    }
                }
            }
        }

        /// <summary>
        /// Process the raw data received from the device into application usable data,
        /// according the the Bluetooth Heart Rate Profile.
        /// https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.heart_rate_measurement.xml&u=org.bluetooth.characteristic.heart_rate_measurement.xml
        /// This function throws an exception if the data cannot be parsed.
        /// </summary>
        /// <param name="data">Raw data received from the heart rate monitor.</param>
        /// <returns>The heart rate measurement value.</returns>
        private static ushort ParseHeartRateValue(byte[] data)
        {
            // Heart Rate profile defined flag values
            const byte heartRateValueFormat = 0x01;

            byte flags = data[0];
            bool isHeartRateValueSizeLong = ((flags & heartRateValueFormat) != 0);

            if (isHeartRateValueSizeLong)
            {
                return BitConverter.ToUInt16(data, 1);
            }
            else
            {
                return data[1];
            }
        }
    }
}
