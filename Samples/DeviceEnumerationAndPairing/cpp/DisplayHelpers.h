// Copyright (c) Microsoft. All rights reserved.

#pragma once
#include "mainpage.xaml.h"
#include "Objbase.h"

namespace SDKTemplate
{
    [Windows::UI::Xaml::Data::Bindable]
    public ref class DeviceSelectorInfo sealed
    {
    public:
        DeviceSelectorInfo(
            Platform::String^ displayName,
            Windows::Devices::Enumeration::DeviceClass deviceClassSelector,
            Platform::String^ selector)
        {
            Initialize(
                displayName,
                deviceClassSelector,
                selector,
                Windows::Devices::Enumeration::DeviceInformationKind::Unknown);
        }

        DeviceSelectorInfo(
            Platform::String^ displayName,
            Windows::Devices::Enumeration::DeviceClass deviceClassSelector,
            Platform::String^ selector,
            Windows::Devices::Enumeration::DeviceInformationKind kind)
        {
            Initialize(
                displayName,
                deviceClassSelector,
                selector,
                kind);
        }

        property Platform::String^ DisplayName;
        property Windows::Devices::Enumeration::DeviceClass DeviceClassSelector;
        property Windows::Devices::Enumeration::DeviceInformationKind Kind;
        property Platform::String^ Selector;

    private:

        void Initialize(
            Platform::String^ displayName,
            Windows::Devices::Enumeration::DeviceClass deviceClassSelector,
            Platform::String^ selector,
            Windows::Devices::Enumeration::DeviceInformationKind kind)
        {
            DisplayName = displayName;
            DeviceClassSelector = deviceClassSelector;
            Kind = kind;
            Selector = selector;
        }
    };


    [Windows::UI::Xaml::Data::Bindable]
    public ref struct DeviceSelectorChoices sealed
    {
    public:

        static property Windows::Foundation::Collections::IVectorView<DeviceSelectorInfo^>^ CommonDeviceSelectors
        {
            Windows::Foundation::Collections::IVectorView<DeviceSelectorInfo^>^ get();
        }

        static property DeviceSelectorInfo^ Bluetooth
        {
            DeviceSelectorInfo^ get();
        }

        static property DeviceSelectorInfo^ BluetoothUnpairedOnly
        {
            DeviceSelectorInfo^ get();
        }

        static property DeviceSelectorInfo^ BluetoothPairedOnly
        {
            DeviceSelectorInfo^ get();
        }

        static property DeviceSelectorInfo^ BluetoothLE
        {
            DeviceSelectorInfo^ get();
        }

        static property DeviceSelectorInfo^ BluetoothLEUnpairedOnly
        {
            DeviceSelectorInfo^ get();
        }

        static property DeviceSelectorInfo^ BluetoothLEPairedOnly
        {
            DeviceSelectorInfo^ get();
        }

        static property DeviceSelectorInfo^ WiFiDirect
        {
            DeviceSelectorInfo^ get();
        }

        static property DeviceSelectorInfo^ WiFiDirectPairedOnly
        {
            DeviceSelectorInfo^ get();
        }

        static property DeviceSelectorInfo^ PointOfServicePrinter
        {
            DeviceSelectorInfo^ get();
        }

        static property DeviceSelectorInfo^ VideoCasting
        {
            DeviceSelectorInfo^ get();
        }

        static property DeviceSelectorInfo^ DialAllApps
        {
            DeviceSelectorInfo^ get();
        }

        static property DeviceSelectorInfo^ Wsd
        {
            DeviceSelectorInfo^ get();
        }

        static property DeviceSelectorInfo^ Upnp
        {
            DeviceSelectorInfo^ get();
        }

        static property Windows::Foundation::Collections::IVectorView<DeviceSelectorInfo^>^ DevicePickerSelectors
        {
            Windows::Foundation::Collections::IVectorView<DeviceSelectorInfo^>^ get();
        }

        static property Windows::Foundation::Collections::IVectorView<DeviceSelectorInfo^>^ FindAllAsyncSelectors
        {
            Windows::Foundation::Collections::IVectorView<DeviceSelectorInfo^>^ get();
        }

        static property Windows::Foundation::Collections::IVectorView<DeviceSelectorInfo^>^ DeviceWatcherSelectors
        {
            Windows::Foundation::Collections::IVectorView<DeviceSelectorInfo^>^ get();
        }

        static property Windows::Foundation::Collections::IVectorView<DeviceSelectorInfo^>^ BackgroundDeviceWatcherSelectors
        {
            Windows::Foundation::Collections::IVectorView<DeviceSelectorInfo^>^ get();
        }

        static property Windows::Foundation::Collections::IVectorView<DeviceSelectorInfo^>^ PairingSelectors
        {
            Windows::Foundation::Collections::IVectorView<DeviceSelectorInfo^>^ get();
        }
    };


    [Windows::UI::Xaml::Data::Bindable]
    public ref struct DeviceInformationKindChoice sealed
    {
    public:
        DeviceInformationKindChoice(
            Platform::String^ displayName,
            Windows::Foundation::Collections::IVectorView<Windows::Devices::Enumeration::DeviceInformationKind>^ deviceInformationKinds)
        {
            DisplayName = displayName;
            DeviceInformationKinds = deviceInformationKinds;
        }
        property Platform::String^ DisplayName;
        property Windows::Foundation::Collections::IVectorView<Windows::Devices::Enumeration::DeviceInformationKind>^ DeviceInformationKinds;
    };


    [Windows::UI::Xaml::Data::Bindable]
    public ref struct DeviceInformationKindChoices sealed
    {
    public:

        static property Windows::Foundation::Collections::IVectorView<DeviceInformationKindChoice^>^ Choices
        {
            Windows::Foundation::Collections::IVectorView<DeviceInformationKindChoice^>^ get();
        }
    };

    [Windows::UI::Xaml::Data::Bindable]
    public ref struct ProtectionLevelSelectorInfo sealed
    {
    public:
        ProtectionLevelSelectorInfo(
            Platform::String^ displayName,
            Windows::Devices::Enumeration::DevicePairingProtectionLevel protectionLevel)
        {
            DisplayName = displayName;
            ProtectionLevel = protectionLevel;
        }
        property Platform::String^ DisplayName;
        property Windows::Devices::Enumeration::DevicePairingProtectionLevel ProtectionLevel;
    };

    [Windows::UI::Xaml::Data::Bindable]
    public ref struct ProtectionSelectorChoices sealed
    {
    public:

        static property Windows::Foundation::Collections::IVectorView<ProtectionLevelSelectorInfo^>^ Selectors
        {
            Windows::Foundation::Collections::IVectorView<ProtectionLevelSelectorInfo^>^ get();
        }
    };

    [Windows::UI::Xaml::Data::Bindable]
    public ref class DeviceInformationDisplay sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:

        DeviceInformationDisplay(Windows::Devices::Enumeration::DeviceInformation^ deviceInfoIn);

        property Windows::Devices::Enumeration::DeviceInformationKind Kind
        {
            Windows::Devices::Enumeration::DeviceInformationKind get() { return deviceInfo->Kind; }
        }

        property Platform::String^ KindString
        {
            Platform::String^ get();
        }

        property Platform::String^ Id
        {
            Platform::String^ get() { return deviceInfo->Id; }
        }

        property Platform::String^ Name
        {
            Platform::String^ get() { return deviceInfo->Name; }
        }

        property Windows::UI::Xaml::Media::Imaging::BitmapImage^ GlyphBitmapImage;

        property Windows::Foundation::Collections::IMapView<Platform::String^, Platform::Object^>^ Properties
        {
            Windows::Foundation::Collections::IMapView<Platform::String^, Platform::Object^>^ get()
            {
                return deviceInfo->Properties;
            }
        }

        property Platform::String^ CanPair
        {
            Platform::String^ get()
            {
                return deviceInfo->Pairing->CanPair ? "True" : "False";
            }
        }

        property Platform::String^ IsPaired
        {
            Platform::String^ get()
            {
                return deviceInfo->Pairing->IsPaired ? "True" : "False";
            }
        }

        property Windows::Devices::Enumeration::DeviceInformation^ DeviceInfo
        {
            Windows::Devices::Enumeration::DeviceInformation^ get() { return deviceInfo; }
        }

        void Update(Windows::Devices::Enumeration::DeviceInformationUpdate^ deviceInfoUpdateIn);

        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

    protected:

        void OnPropertyChanged(Platform::String^ name);

    private:

        void UpdateGlyphBitmapImage();
        
        Windows::Devices::Enumeration::DeviceInformation^ deviceInfo;
    };

    public ref class GeneralPropertyValueConverter sealed : Windows::UI::Xaml::Data::IValueConverter
    {
    public:

        virtual Platform::Object^ Convert(
            Platform::Object^ value, 
            Windows::UI::Xaml::Interop::TypeName targetType,
            Platform::Object^ parameter, 
            Platform::String^ language);

        virtual Platform::Object^ ConvertBack(
            Platform::Object^ value, 
            Windows::UI::Xaml::Interop::TypeName targetType,
            Platform::Object^ parameter,
            Platform::String^ language)
        {
            throw ref new Platform::NotImplementedException();
        }
    };
}