// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Collections.Generic;
using System.ComponentModel;
using Windows.Devices.Enumeration;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Media.Imaging;

// NOTE: The following using statements are only needed in order to demonstrate many of the
// different device selectors available from Windows Runtime APIs. You will only need to include
// the namespace for the Windows Runtime API your actual scenario needs.
using Windows.Devices.HumanInterfaceDevice;
using Windows.Networking.Proximity;
using Windows.Devices.Bluetooth;
using Windows.Devices.WiFiDirect;
using Windows.Media.Casting;
using Windows.Media.DialProtocol;
using Windows.Devices.Sensors;
using Windows.Devices.PointOfService;

namespace SDKTemplate
{
    public class DeviceSelectorInfo
    {
        public string DisplayName { get; set; }
        public DeviceClass DeviceClassSelector { get; set; } = DeviceClass.All;
        public DeviceInformationKind Kind { get; set; } = DeviceInformationKind.Unknown;
        public string Selector { get; set; }
    }

    public static class DeviceSelectorChoices
    {

        public static List<DeviceSelectorInfo> CommonDeviceSelectors => new List<DeviceSelectorInfo>()
        {
            // Pre-canned device class selectors
            new DeviceSelectorInfo() { DisplayName = "All Device Interfaces (default)", DeviceClassSelector = DeviceClass.All, Selector = null },
            new DeviceSelectorInfo() { DisplayName = "Audio Capture", DeviceClassSelector = DeviceClass.AudioCapture, Selector = null },
            new DeviceSelectorInfo() { DisplayName = "Audio Render", DeviceClassSelector = DeviceClass.AudioRender, Selector = null },
            new DeviceSelectorInfo() { DisplayName = "Image Scanner", DeviceClassSelector = DeviceClass.ImageScanner, Selector = null },
            new DeviceSelectorInfo() { DisplayName = "Location", DeviceClassSelector = DeviceClass.Location, Selector = null },
            new DeviceSelectorInfo() { DisplayName = "Portable Storage", DeviceClassSelector = DeviceClass.PortableStorageDevice, Selector = null },
            new DeviceSelectorInfo() { DisplayName = "Video Capture", DeviceClassSelector = DeviceClass.VideoCapture, Selector = null },

            // A few examples of selectors built dynamically by windows runtime apis.
            new DeviceSelectorInfo() { DisplayName = "Human Interface (HID)", Selector = HidDevice.GetDeviceSelector(0, 0) },
            new DeviceSelectorInfo() { DisplayName = "Activity Sensor", Selector = ActivitySensor.GetDeviceSelector() },
            new DeviceSelectorInfo() { DisplayName = "Pedometer", Selector = Pedometer.GetDeviceSelector() },
            new DeviceSelectorInfo() { DisplayName = "Proximity", Selector = ProximityDevice.GetDeviceSelector() },
            new DeviceSelectorInfo() { DisplayName = "Proximity Sensor", Selector = ProximitySensor.GetDeviceSelector() },
        };

        // Currently Bluetooth APIs don't provide a selector to get ALL devices that are both paired and non-paired.
        // Typically you wouldn't need this for common scenarios, but it's convenient to demonstrate the
        // various sample scenarios.
        public static DeviceSelectorInfo Bluetooth =>
            new DeviceSelectorInfo() { DisplayName = "Bluetooth", Selector = "System.Devices.Aep.ProtocolId:=\"{e0cbf06c-cd8b-4647-bb8a-263b43f0f974}\"", Kind = DeviceInformationKind.AssociationEndpoint };

        public static DeviceSelectorInfo BluetoothUnpairedOnly =>
            new DeviceSelectorInfo() { DisplayName = "Bluetooth (unpaired)", Selector = BluetoothDevice.GetDeviceSelectorFromPairingState(false) };

        public static DeviceSelectorInfo BluetoothPairedOnly =>
            new DeviceSelectorInfo() { DisplayName = "Bluetooth (paired)", Selector = BluetoothDevice.GetDeviceSelectorFromPairingState(true) };

        // Currently Bluetooth APIs don't provide a selector to get ALL devices that are both paired and non-paired.
        // Typically you wouldn't need this for common scenarios, but it's convenient to demonstrate the
        // various sample scenarios.
        public static DeviceSelectorInfo BluetoothLE =>
            new DeviceSelectorInfo() { DisplayName = "Bluetooth LE", Selector = "System.Devices.Aep.ProtocolId:=\"{bb7bb05e-5972-42b5-94fc-76eaa7084d49}\"", Kind = DeviceInformationKind.AssociationEndpoint };

        public static DeviceSelectorInfo BluetoothLEUnpairedOnly =>
            new DeviceSelectorInfo() { DisplayName = "Bluetooth LE (unpaired)", Selector = BluetoothLEDevice.GetDeviceSelectorFromPairingState(false) };

        public static DeviceSelectorInfo BluetoothLEPairedOnly =>
            new DeviceSelectorInfo() { DisplayName = "Bluetooth LE (paired)", Selector = BluetoothLEDevice.GetDeviceSelectorFromPairingState(true) };

        public static DeviceSelectorInfo WiFiDirect =>
            new DeviceSelectorInfo() { DisplayName = "Wi-Fi Direct", Selector = WiFiDirectDevice.GetDeviceSelector(WiFiDirectDeviceSelectorType.AssociationEndpoint) };

        public static DeviceSelectorInfo WiFiDirectPairedOnly =>
            new DeviceSelectorInfo() { DisplayName = "Wi-Fi Direct (paired)", Selector = WiFiDirectDevice.GetDeviceSelector() };

        public static DeviceSelectorInfo PointOfServicePrinter =>
            new DeviceSelectorInfo() { DisplayName = "Point of Service Printer", Selector = PosPrinter.GetDeviceSelector() };

        public static DeviceSelectorInfo VideoCasting =>
            new DeviceSelectorInfo() { DisplayName = "Video Casting", Selector = CastingDevice.GetDeviceSelector(CastingPlaybackTypes.Video) };

        public static DeviceSelectorInfo DialAllApps =>
            new DeviceSelectorInfo() { DisplayName = "DIAL (All apps)", Selector = DialDevice.GetDeviceSelector("") };

        // WSD and UPnP are unique in that there are currently no general WSD or UPnP APIs to USE the devices once you've discovered them.
        // You can pair the devices using DeviceInformation.Pairing.PairAsync etc, and you can USE them with the sockets APIs. However, since
        // there's no specific API right now, there's no *.GetDeviceSelector available.  That's why we just simply build the device selector
        // ourselves and specify the correct DeviceInformationKind (AssociationEndpoint).
        public static DeviceSelectorInfo Wsd =>
            new DeviceSelectorInfo() { DisplayName = "Web Services on Devices (WSD)", Selector = "System.Devices.Aep.ProtocolId:=\"{782232aa-a2f9-4993-971b-aedc551346b0}\"", Kind = DeviceInformationKind.AssociationEndpoint };

        public static DeviceSelectorInfo Upnp =>
            new DeviceSelectorInfo() { DisplayName = "UPnP", Selector = "System.Devices.Aep.ProtocolId:=\"{0e261de4-12f0-46e6-91ba-428607ccef64}\"", Kind = DeviceInformationKind.AssociationEndpoint };

        public static DeviceSelectorInfo NetworkCamera =>
            new DeviceSelectorInfo() { DisplayName = "Web Services on Devices (NetworkCamera)", Selector = "System.Devices.Aep.ProtocolId:=\"{43cc0de4-8ca8-4a56-805a-86fc63f21602}\"", Kind = DeviceInformationKind.AssociationEndpoint };

        public static void AddVideoCastingIfSupported(List<DeviceSelectorInfo> selectors)
        {
            try
            {
                selectors.Add(VideoCasting);
            }
            catch (Exception ex) when (ex.HResult == unchecked((int)0x80070032)) // HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED)
            {
                // Video casting is not supported by the system.
            }
        }

        public static List<DeviceSelectorInfo> DevicePickerSelectors
        {
            get
            {
                // Add all selectors that can be used with the DevicePicker
                List<DeviceSelectorInfo> selectors = new List<DeviceSelectorInfo>(CommonDeviceSelectors);
                selectors.Add(BluetoothPairedOnly);
                selectors.Add(BluetoothUnpairedOnly);
                selectors.Add(BluetoothLEPairedOnly);
                selectors.Add(BluetoothLEUnpairedOnly);
                selectors.Add(WiFiDirect);
                selectors.Add(PointOfServicePrinter);
                AddVideoCastingIfSupported(selectors);
                selectors.Add(DialAllApps);

                return selectors;
            }
        }

        // Add all selectors that are reasonable to use with FindAllAsync
        public static List<DeviceSelectorInfo> FindAllAsyncSelectors =>
            new List<DeviceSelectorInfo>(CommonDeviceSelectors)
            {
                BluetoothPairedOnly,
                BluetoothLEPairedOnly,
                WiFiDirectPairedOnly
            };

        public static List<DeviceSelectorInfo> DeviceWatcherSelectors
        {
            get
            {
                // Add all selectors that can be used with the DeviceWatcher
                List<DeviceSelectorInfo> selectors = new List<DeviceSelectorInfo>(CommonDeviceSelectors);
                selectors.Add(Bluetooth);
                selectors.Add(BluetoothLE);
                selectors.Add(WiFiDirect);
                selectors.Add(PointOfServicePrinter);
                AddVideoCastingIfSupported(selectors);
                selectors.Add(DialAllApps);
                selectors.Add(Wsd);
                selectors.Add(Upnp);

                return selectors;
            }
        }

        public static List<DeviceSelectorInfo> BackgroundDeviceWatcherSelectors
        {
            get
            {
                // Add all selectors that can be used with the BackgroundDeviceWatcher
                List<DeviceSelectorInfo> selectors = new List<DeviceSelectorInfo>(CommonDeviceSelectors);
                selectors.Add(BluetoothPairedOnly);
                selectors.Add(BluetoothLEPairedOnly);
                selectors.Add(WiFiDirectPairedOnly);
                selectors.Add(PointOfServicePrinter);
                AddVideoCastingIfSupported(selectors);
                selectors.Add(DialAllApps);
                selectors.Add(Wsd);
                selectors.Add(Upnp);

                return selectors;
            }
        }

        /// <summary>
        /// Selectors for use in the pairing scenarios
        /// </summary>
        public static List<DeviceSelectorInfo> PairingSelectors
        {
            get
            {
                // Add selectors that can be used in pairing scenarios
                List<DeviceSelectorInfo> selectors = new List<DeviceSelectorInfo>();
                selectors.Add(Bluetooth);
                selectors.Add(BluetoothLE);
                selectors.Add(WiFiDirect);
                selectors.Add(PointOfServicePrinter);
                AddVideoCastingIfSupported(selectors);
                selectors.Add(Wsd);
                selectors.Add(Upnp);
                selectors.Add(NetworkCamera);

                return selectors;
            }
        }
    }

    public struct DeviceInformationKindChoice
    {
        public string DisplayName { get; set; }
        public DeviceInformationKind[] DeviceInformationKinds { get; set; }
    }

    public static class DeviceInformationKindChoices
    {
        public static List<DeviceInformationKindChoice> Choices =>
            new List<DeviceInformationKindChoice>()
            {
                new DeviceInformationKindChoice() { DisplayName = "DeviceContainer", DeviceInformationKinds = new[] { DeviceInformationKind.DeviceContainer } },
                new DeviceInformationKindChoice() { DisplayName = "Device", DeviceInformationKinds = new[] { DeviceInformationKind.Device } },
                new DeviceInformationKindChoice() { DisplayName = "DeviceInterface (Default)", DeviceInformationKinds = new[] { DeviceInformationKind.DeviceInterface } },
                new DeviceInformationKindChoice() { DisplayName = "DeviceInterfaceClass", DeviceInformationKinds = new[] { DeviceInformationKind.DeviceInterfaceClass } },
                new DeviceInformationKindChoice() { DisplayName = "AssociationEndpointContainer", DeviceInformationKinds = new[] { DeviceInformationKind.AssociationEndpointContainer } },
                new DeviceInformationKindChoice() { DisplayName = "AssociationEndpoint", DeviceInformationKinds = new[] { DeviceInformationKind.AssociationEndpoint } },
                new DeviceInformationKindChoice() { DisplayName = "AssociationEndpointService", DeviceInformationKinds = new[] { DeviceInformationKind.AssociationEndpointService } },
                new DeviceInformationKindChoice() { DisplayName = "AssociationEndpointService and DeviceInterface", DeviceInformationKinds = new[] { DeviceInformationKind.AssociationEndpointService, DeviceInformationKind.DeviceInterface } },
            };
    }

    public struct ProtectionLevelSelectorInfo
    {
        public string DisplayName { get; set; }
        public DevicePairingProtectionLevel ProtectionLevel { get; set; }
    }

    public static class ProtectionSelectorChoices
    {
        public static List<ProtectionLevelSelectorInfo> Selectors =>
            new List<ProtectionLevelSelectorInfo>()
            {
                new ProtectionLevelSelectorInfo() { DisplayName = "Default", ProtectionLevel = DevicePairingProtectionLevel.Default },
                new ProtectionLevelSelectorInfo() { DisplayName = "None", ProtectionLevel = DevicePairingProtectionLevel.None },
                new ProtectionLevelSelectorInfo() { DisplayName = "Encryption", ProtectionLevel = DevicePairingProtectionLevel.Encryption },
                new ProtectionLevelSelectorInfo() { DisplayName = "Encryption and authentication", ProtectionLevel = DevicePairingProtectionLevel.EncryptionAndAuthentication },
            };
    }

    public class DeviceInformationDisplay : INotifyPropertyChanged
    {
        public DeviceInformationDisplay(DeviceInformation deviceInfoIn)
        {
            DeviceInformation = deviceInfoIn;
            UpdateGlyphBitmapImage();
        }

        public DeviceInformationKind Kind => DeviceInformation.Kind;
        public string Id => DeviceInformation.Id;
        public string Name => DeviceInformation.Name;
        public BitmapImage GlyphBitmapImage { get; private set; }
        public bool CanPair => DeviceInformation.Pairing.CanPair;
        public bool IsPaired => DeviceInformation.Pairing.IsPaired;
        public IReadOnlyDictionary<string, object> Properties => DeviceInformation.Properties;
        public DeviceInformation DeviceInformation { get; private set; }

        public void Update(DeviceInformationUpdate deviceInfoUpdate)
        {
            DeviceInformation.Update(deviceInfoUpdate);

            OnPropertyChanged("Kind");
            OnPropertyChanged("Id");
            OnPropertyChanged("Name");
            OnPropertyChanged("DeviceInformation");
            OnPropertyChanged("CanPair");
            OnPropertyChanged("IsPaired");
            OnPropertyChanged("GetPropertyForDisplay");

            UpdateGlyphBitmapImage();
        }

        public string GetPropertyForDisplay(string key) => Properties[key]?.ToString();

        private async void UpdateGlyphBitmapImage()
        {
            DeviceThumbnail deviceThumbnail = await DeviceInformation.GetGlyphThumbnailAsync();
            BitmapImage glyphBitmapImage = new BitmapImage();
            await glyphBitmapImage.SetSourceAsync(deviceThumbnail);
            GlyphBitmapImage = glyphBitmapImage;
            OnPropertyChanged("GlyphBitmapImage");
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged(string name) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
    }
}
