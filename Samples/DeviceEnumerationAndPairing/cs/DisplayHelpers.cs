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

namespace DeviceEnumeration
{
    public class DeviceSelectorInfo
    {
        public DeviceSelectorInfo()
        {
            Kind = DeviceInformationKind.Unknown;
            DeviceClassSelector = DeviceClass.All;
        }

        public string DisplayName
        {
            get;
            set;
        }

        public DeviceClass DeviceClassSelector
        {
            get;
            set;
        }

        public DeviceInformationKind Kind
        {
            get;
            set;
        }

        public string Selector
        {
            get;
            set;
        }
    }

    public static class DeviceSelectorChoices
    {
        
        public static List<DeviceSelectorInfo> CommonDeviceSelectors
        {
            get
            {
                List<DeviceSelectorInfo> selectors = new List<DeviceSelectorInfo>();

                // Pre-canned device class selectors
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "All Device Interfaces (default)", DeviceClassSelector = DeviceClass.All, Selector = null });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Audio Capture", DeviceClassSelector = DeviceClass.AudioCapture, Selector = null });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Audio Render", DeviceClassSelector = DeviceClass.AudioRender, Selector = null });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Image Scanner", DeviceClassSelector = DeviceClass.ImageScanner, Selector = null });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Location", DeviceClassSelector = DeviceClass.Location, Selector = null });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Portable Storage", DeviceClassSelector = DeviceClass.PortableStorageDevice, Selector = null });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Video Capture", DeviceClassSelector = DeviceClass.VideoCapture, Selector = null });

                // A few examples of selectors built dynamically by windows runtime apis. 
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Human Interface (HID)", Selector = HidDevice.GetDeviceSelector(0, 0) });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Activity Sensor", Selector = ActivitySensor.GetDeviceSelector() });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Pedometer", Selector = Pedometer.GetDeviceSelector() });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Proximity", Selector = ProximityDevice.GetDeviceSelector() });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Proximity Sensor", Selector = ProximitySensor.GetDeviceSelector() });

                return selectors;
            }
        }

        public static DeviceSelectorInfo Bluetooth
        {
            get
            {
                // Currently Bluetooth APIs don't provide a selector to get ALL devices that are both paired and non-paired.  Typically you wouldn't need this for common scenarios, but it's convenient to demonstrate the
                // various sample scenarios. 
                return new DeviceSelectorInfo() { DisplayName = "Bluetooth", Selector = "System.Devices.Aep.ProtocolId:=\"{e0cbf06c-cd8b-4647-bb8a-263b43f0f974}\"", Kind = DeviceInformationKind.AssociationEndpoint };
            }
        }

        public static DeviceSelectorInfo BluetoothUnpairedOnly
        {
            get
            {
                return new DeviceSelectorInfo() { DisplayName = "Bluetooth (unpaired)", Selector = BluetoothDevice.GetDeviceSelectorFromPairingState(false) };
            }
        }

        public static DeviceSelectorInfo BluetoothPairedOnly
        {
            get
            {
                return new DeviceSelectorInfo() { DisplayName = "Bluetooth (paired)", Selector = BluetoothDevice.GetDeviceSelectorFromPairingState(true) };
            }
        }

        public static DeviceSelectorInfo BluetoothLE
        {
            get
            {
                // Currently Bluetooth APIs don't provide a selector to get ALL devices that are both paired and non-paired.  Typically you wouldn't need this for common scenarios, but it's convenient to demonstrate the
                // various sample scenarios. 
                return new DeviceSelectorInfo() { DisplayName = "Bluetooth LE", Selector = "System.Devices.Aep.ProtocolId:=\"{bb7bb05e-5972-42b5-94fc-76eaa7084d49}\"", Kind = DeviceInformationKind.AssociationEndpoint };
            }
        }

        public static DeviceSelectorInfo BluetoothLEUnpairedOnly
        {
            get
            {
                return new DeviceSelectorInfo() { DisplayName = "Bluetooth LE (unpaired)", Selector = BluetoothLEDevice.GetDeviceSelectorFromPairingState(false) };
            }
        }

        public static DeviceSelectorInfo BluetoothLEPairedOnly
        {
            get
            {
                return new DeviceSelectorInfo() { DisplayName = "Bluetooth LE (paired)", Selector = BluetoothLEDevice.GetDeviceSelectorFromPairingState(true) };
            }
        }

        public static DeviceSelectorInfo WiFiDirect
        {
            get
            {
                return new DeviceSelectorInfo() { DisplayName = "Wi-Fi Direct", Selector = WiFiDirectDevice.GetDeviceSelector(WiFiDirectDeviceSelectorType.AssociationEndpoint) };
            }
        }

        public static DeviceSelectorInfo WiFiDirectPairedOnly
        {
            get
            {
                return new DeviceSelectorInfo() { DisplayName = "Wi-Fi Direct (paired)", Selector = WiFiDirectDevice.GetDeviceSelector() };
            }
        }
        
        public static DeviceSelectorInfo PointOfServicePrinter
        {
            get
            {
                return new DeviceSelectorInfo() { DisplayName = "Point of Service Printer", Selector = PosPrinter.GetDeviceSelector() };
            }
        }

        public static DeviceSelectorInfo VideoCasting
        {
            get
            {
                return new DeviceSelectorInfo() { DisplayName = "Video Casting", Selector = CastingDevice.GetDeviceSelector(CastingPlaybackTypes.Video) };
            }
        }

        public static DeviceSelectorInfo DialAllApps
        {
            get
            {
                return new DeviceSelectorInfo() { DisplayName = "DIAL (All apps)", Selector = DialDevice.GetDeviceSelector("") };
            }
        }

        // WSD and UPnP are unique in that there are currently no general WSD or UPnP APIs to USE the devices once you've discovered them. 
        // You can pair the devices using DeviceInformation.Pairing.PairAsync etc, and you can USE them with the sockets APIs. However, since
        // there's no specific API right now, there's no *.GetDeviceSelector available.  That's why we just simply build the device selector
        // ourselves and specify the correct DeviceInformationKind (AssociationEndpoint). 
        public static DeviceSelectorInfo Wsd
        {
            get
            {
                return new DeviceSelectorInfo() { DisplayName = "Web Services on Devices (WSD)", Selector = "System.Devices.Aep.ProtocolId:=\"{782232aa-a2f9-4993-971b-aedc551346b0}\"", Kind = DeviceInformationKind.AssociationEndpoint };
            }
        }

        public static DeviceSelectorInfo Upnp
        {
            get
            {
                return new DeviceSelectorInfo() { DisplayName = "UPnP", Selector = "System.Devices.Aep.ProtocolId:=\"{0e261de4-12f0-46e6-91ba-428607ccef64}\"", Kind = DeviceInformationKind.AssociationEndpoint };
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
                selectors.Add(VideoCasting);
                selectors.Add(DialAllApps);

                return selectors;
            }
        }

        public static List<DeviceSelectorInfo> FindAllAsyncSelectors
        {
            get
            {
                // Add all selectors that are reasonable to use with FindAllAsync
                List<DeviceSelectorInfo> selectors = new List<DeviceSelectorInfo>(CommonDeviceSelectors);
                selectors.Add(BluetoothPairedOnly);
                selectors.Add(BluetoothLEPairedOnly);
                selectors.Add(WiFiDirectPairedOnly);

                return selectors;
            }
        }
        
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
                selectors.Add(VideoCasting);
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
                selectors.Add(VideoCasting);
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
                selectors.Add(VideoCasting);
                selectors.Add(Wsd);
                selectors.Add(Upnp);

                return selectors;
            }
        }
    }

    public struct DeviceInformationKindChoice
    {
        public string DisplayName
        {
            get;
            set;
        }

        public DeviceInformationKind[] DeviceInformationKinds
        {
            get;
            set;
        }
    }

    public static class DeviceInformationKindChoices
    {
        public static List<DeviceInformationKindChoice> Choices
        {
            get
            {
                List<DeviceInformationKindChoice> choices = new List<DeviceInformationKindChoice>();

                choices.Add(new DeviceInformationKindChoice() { DisplayName = "DeviceContainer", DeviceInformationKinds = new DeviceInformationKind[] { DeviceInformationKind.DeviceContainer } });
                choices.Add(new DeviceInformationKindChoice() { DisplayName = "Device", DeviceInformationKinds = new DeviceInformationKind[] { DeviceInformationKind.Device } });
                choices.Add(new DeviceInformationKindChoice() { DisplayName = "DeviceInterface (Default)", DeviceInformationKinds = new DeviceInformationKind[] { DeviceInformationKind.DeviceInterface } });
                choices.Add(new DeviceInformationKindChoice() { DisplayName = "DeviceInterfaceClass", DeviceInformationKinds = new DeviceInformationKind[] { DeviceInformationKind.DeviceInterfaceClass } });
                choices.Add(new DeviceInformationKindChoice() { DisplayName = "AssociationEndpointContainer", DeviceInformationKinds = new DeviceInformationKind[] { DeviceInformationKind.AssociationEndpointContainer } });
                choices.Add(new DeviceInformationKindChoice() { DisplayName = "AssociationEndpoint", DeviceInformationKinds = new DeviceInformationKind[] { DeviceInformationKind.AssociationEndpoint } });
                choices.Add(new DeviceInformationKindChoice() { DisplayName = "AssociationEndpointService", DeviceInformationKinds = new DeviceInformationKind[] { DeviceInformationKind.AssociationEndpointService } });
                choices.Add(new DeviceInformationKindChoice() { DisplayName = "AssociationEndpointService and DeviceInterface", DeviceInformationKinds = new DeviceInformationKind[] { DeviceInformationKind.AssociationEndpointService, DeviceInformationKind.DeviceInterface} });

                return choices;
            }
        }
    }

    public struct ProtectionLevelSelectorInfo
    {
        public string DisplayName
        {
            get;
            set;
        }

        public Windows.Devices.Enumeration.DevicePairingProtectionLevel ProtectionLevel
        {
            get;
            set;
        }
    }

    public static class ProtectionSelectorChoices
    {
        public static List<ProtectionLevelSelectorInfo> Selectors
        {
            get
            {
                List<ProtectionLevelSelectorInfo> selectors = new List<ProtectionLevelSelectorInfo>();
                selectors.Add(new ProtectionLevelSelectorInfo() { DisplayName = "Default", ProtectionLevel= Windows.Devices.Enumeration.DevicePairingProtectionLevel.Default});
                selectors.Add(new ProtectionLevelSelectorInfo() { DisplayName = "None", ProtectionLevel = Windows.Devices.Enumeration.DevicePairingProtectionLevel.None});
                selectors.Add(new ProtectionLevelSelectorInfo() { DisplayName = "Encryption", ProtectionLevel = Windows.Devices.Enumeration.DevicePairingProtectionLevel.Encryption});
                selectors.Add(new ProtectionLevelSelectorInfo() { DisplayName = "Encryption and authentication", ProtectionLevel = Windows.Devices.Enumeration.DevicePairingProtectionLevel.EncryptionAndAuthentication});

                return selectors;
            }
        }
    }

    public class DeviceInformationDisplay : INotifyPropertyChanged
    {
        private DeviceInformation deviceInfo;

        public DeviceInformationDisplay(DeviceInformation deviceInfoIn)
        {
            deviceInfo = deviceInfoIn;
            UpdateGlyphBitmapImage();
        }

        public DeviceInformationKind Kind
        {
            get
            {
                return deviceInfo.Kind;
            }
        }

        public string Id
        {
            get
            {
                return deviceInfo.Id;
            }
        }

        public string Name
        {
            get
            {
                return deviceInfo.Name;
            }
        }

        public BitmapImage GlyphBitmapImage
        {
            get;
            private set;
        }

        public bool CanPair
        {
            get
            {
                return deviceInfo.Pairing.CanPair;
            }
        }

        public bool IsPaired
        {
            get
            {
                return deviceInfo.Pairing.IsPaired;
            }
        }

        public IReadOnlyDictionary<string, object> Properties
        {
            get
            {
                return deviceInfo.Properties;
            }
        }

        public DeviceInformation DeviceInformation
        {
            get
            {
                return deviceInfo;
            }

            private set
            {
                deviceInfo = value;
            }
        }

        public void Update(DeviceInformationUpdate deviceInfoUpdate)
        {
            deviceInfo.Update(deviceInfoUpdate);

            OnPropertyChanged("Kind");
            OnPropertyChanged("Id");
            OnPropertyChanged("Name");
            OnPropertyChanged("DeviceInformation");
            OnPropertyChanged("CanPair");
            OnPropertyChanged("IsPaired");

            UpdateGlyphBitmapImage();
        }

        private async void UpdateGlyphBitmapImage()
        {
            DeviceThumbnail deviceThumbnail = await deviceInfo.GetGlyphThumbnailAsync();
            BitmapImage glyphBitmapImage = new BitmapImage();
            await glyphBitmapImage.SetSourceAsync(deviceThumbnail);
            GlyphBitmapImage = glyphBitmapImage;
            OnPropertyChanged("GlyphBitmapImage");
        }
        
        public event PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged(string name)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(name));
            }
        }
    }

    public class GeneralPropertyValueConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            object property = null;

            if (value is IReadOnlyDictionary<string, object> &&
                parameter is string &&
                !String.IsNullOrEmpty((string)parameter))
            {
                IReadOnlyDictionary<string, object> properties = value as IReadOnlyDictionary<string, object>;
                string propertyName = parameter as string;

                property = properties[propertyName];
            }

            return property;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
