// Copyright (c) Microsoft. All rights reserved.

#pragma once
#include "mainpage.xaml.h"
#include "Objbase.h"

namespace DeviceEnumerationCpp
{
    [Windows::UI::Xaml::Data::Bindable]
    public ref struct DeviceSelectorInfo sealed
    {
    public:
        DeviceSelectorInfo(
            Platform::String^ displayName,
            Windows::Devices::Enumeration::DeviceClass deviceClassSelector,
            Platform::String^ selector
            )
        {
            DisplayName = displayName;
            DeviceClassSelector = deviceClassSelector;
            Selector = selector;
        }
        property Platform::String^ DisplayName;
        property Windows::Devices::Enumeration::DeviceClass DeviceClassSelector;
        property Platform::String^ Selector;
    };


    [Windows::UI::Xaml::Data::Bindable]
    public ref struct DeviceSelectorChoices sealed
    {
    public:

        static property Windows::Foundation::Collections::IVectorView<DeviceSelectorInfo^>^ Selectors
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
            Windows::Foundation::Collections::IVectorView<Windows::Devices::Enumeration::DeviceInformationKind>^ deviceInformationKinds
            )
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
    public ref struct ProtocolSelectorInfo sealed
    {
    public:
        ProtocolSelectorInfo(
            Platform::String^ displayName,
            Platform::String^ selector
            )
        {
            DisplayName = displayName;
            Selector = selector;
        }
        property Platform::String^ DisplayName;
        property Platform::String^ Selector;
    };


    [Windows::UI::Xaml::Data::Bindable]
    public ref struct ProtocolSelectorChoices sealed
    {
    public:

        static property Windows::Foundation::Collections::IVectorView<ProtocolSelectorInfo^>^ Selectors
        {
            Windows::Foundation::Collections::IVectorView<ProtocolSelectorInfo^>^ get();
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