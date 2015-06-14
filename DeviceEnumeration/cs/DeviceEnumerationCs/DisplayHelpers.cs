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
using Windows.Devices.Bluetooth.Rfcomm;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Devices.WiFiDirect;

namespace DeviceEnumeration
{
    public struct DeviceSelectorInfo
    {
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

        public string Selector
        {
            get;
            set;
        }
    }

    public static class DeviceSelectorChoices
    {        
        // The Chat Server's custom service Uuid: 34B1CF4D-1069-4AD6-89B6-E161D79BE4D8          
        private static readonly Guid RfcommChatServiceUuid = Guid.Parse("34B1CF4D-1069-4AD6-89B6-E161D79BE4D8"); 
        
        public static List<DeviceSelectorInfo> Selectors
        {
            get
            {
                List<DeviceSelectorInfo> selectors = new List<DeviceSelectorInfo>();

                // Pre-canned device class selectors
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "All Device Interfaces", DeviceClassSelector = DeviceClass.All, Selector = null });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Audio Capture Devices", DeviceClassSelector = DeviceClass.AudioCapture, Selector = null });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Audio Render Devices", DeviceClassSelector = DeviceClass.AudioRender, Selector = null });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Image Scanner Devices", DeviceClassSelector = DeviceClass.ImageScanner, Selector = null });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Location Devices", DeviceClassSelector = DeviceClass.Location, Selector = null });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Portable Storage Devices", DeviceClassSelector = DeviceClass.PortableStorageDevice, Selector = null });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Video Capture Devices", DeviceClassSelector = DeviceClass.VideoCapture, Selector = null });

                // A few examples of selectors built dynamically by windows runtime apis. 
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Proximity Devices", Selector = ProximityDevice.GetDeviceSelector() });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Human Interface Devices", Selector = HidDevice.GetDeviceSelector(0, 0) });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Bluetooth Rfcomm Chat Services", Selector = RfcommDeviceService.GetDeviceSelector(RfcommServiceId.FromUuid(RfcommChatServiceUuid)) });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "Bluetooth Gatt Heart Rate Devices", Selector = GattDeviceService.GetDeviceSelectorFromUuid(GattServiceUuids.HeartRate) });
                selectors.Add(new DeviceSelectorInfo() { DisplayName = "WiFi Direct Devices", Selector = WiFiDirectDevice.GetDeviceSelector() });

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

    public struct ProtocolSelectorInfo
    {
        public string DisplayName
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

    public static class ProtocolSelectorChoices
    {
        public static List<ProtocolSelectorInfo> Selectors
        {
            get
            {
                List<ProtocolSelectorInfo> selectors = new List<ProtocolSelectorInfo>();
                
                selectors.Add(new ProtocolSelectorInfo() { DisplayName = "Wi-Fi Direct", Selector = "System.Devices.Aep.ProtocolId:=\"{0407d24e-53de-4c9a-9ba1-9ced54641188}\"" });
                selectors.Add(new ProtocolSelectorInfo() { DisplayName = "WSD", Selector = "System.Devices.Aep.ProtocolId:=\"{782232aa-a2f9-4993-971b-aedc551346b0}\"" });
                selectors.Add(new ProtocolSelectorInfo() { DisplayName = "UPnP", Selector = "System.Devices.Aep.ProtocolId:=\"{0e261de4-12f0-46e6-91ba-428607ccef64}\"" });
                selectors.Add(new ProtocolSelectorInfo() { DisplayName = "Point of Service", Selector = "System.Devices.Aep.ProtocolId:=\"{D4BF61B3-442E-4ADA-882D-FA7B70C832D9}\"" });

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
                false == String.IsNullOrEmpty((string)parameter))
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
