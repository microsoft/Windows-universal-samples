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

#pragma once
#include "DeviceSelectorInfo.g.h"
#include "DeviceInformationKindChoice.g.h"
#include "ProtectionLevelSelectorInfo.g.h"
#include "DeviceInformationDisplay.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct DeviceSelectorInfo : DeviceSelectorInfoT<DeviceSelectorInfo>
    {
        DeviceSelectorInfo(
            hstring displayName,
            Windows::Devices::Enumeration::DeviceClass deviceClassSelector,
            Windows::Devices::Enumeration::DeviceInformationKind kind = Windows::Devices::Enumeration::DeviceInformationKind::Unknown,
            hstring selector = {})
            : m_displayName(std::move(displayName)),
            m_deviceClassSelector(deviceClassSelector),
            m_kind(kind),
            m_selector(std::move(selector))
        {
        }

        DeviceSelectorInfo(
            hstring const& displayName,
            hstring const& selector,
            Windows::Devices::Enumeration::DeviceInformationKind kind = Windows::Devices::Enumeration::DeviceInformationKind::Unknown)
            : DeviceSelectorInfo(displayName, {}, kind, selector)
        {
        }

        hstring DisplayName() const
        {
            return m_displayName;
        }

        Windows::Devices::Enumeration::DeviceClass DeviceClassSelector() const
        {
            return m_deviceClassSelector;
        }

        Windows::Devices::Enumeration::DeviceInformationKind Kind() const
        {
            return m_kind;
        }

        hstring Selector() const
        {
            return m_selector;
        }

    private:
        hstring m_displayName;
        Windows::Devices::Enumeration::DeviceClass m_deviceClassSelector;
        Windows::Devices::Enumeration::DeviceInformationKind m_kind;
        hstring m_selector;
    };

    struct DeviceSelectorChoices
    {
        static Windows::Foundation::Collections::IObservableVector<SDKTemplate::DeviceSelectorInfo> CommonDeviceSelectors();
        static Windows::Foundation::Collections::IObservableVector<SDKTemplate::DeviceSelectorInfo> DevicePickerSelectors();
        static Windows::Foundation::Collections::IObservableVector<SDKTemplate::DeviceSelectorInfo> FindAllAsyncSelectors();
        static Windows::Foundation::Collections::IObservableVector<SDKTemplate::DeviceSelectorInfo> DeviceWatcherSelectors();
        static Windows::Foundation::Collections::IObservableVector<SDKTemplate::DeviceSelectorInfo> BackgroundDeviceWatcherSelectors();
        static Windows::Foundation::Collections::IObservableVector<SDKTemplate::DeviceSelectorInfo> PairingSelectors();
    };

    struct DeviceInformationKindChoice : DeviceInformationKindChoiceT<DeviceInformationKindChoice>
    {
    public:
        DeviceInformationKindChoice(hstring displayName, std::vector<Windows::Devices::Enumeration::DeviceInformationKind> kinds)
            : displayName(std::move(displayName)),
            kinds(std::move(kinds))
        {
        }

        hstring DisplayName() const
        {
            return displayName;
        }

        std::vector<Windows::Devices::Enumeration::DeviceInformationKind> const& Kinds() const
        {
            return kinds;
        }

    private:
        hstring displayName;
        std::vector<Windows::Devices::Enumeration::DeviceInformationKind> kinds;
    };

    struct DeviceInformationKindChoices
    {
        static Windows::Foundation::Collections::IObservableVector<SDKTemplate::DeviceInformationKindChoice> Choices();
    };

    struct ProtectionLevelSelectorInfo : ProtectionLevelSelectorInfoT<ProtectionLevelSelectorInfo>
    {
        ProtectionLevelSelectorInfo(hstring displayName, Windows::Devices::Enumeration::DevicePairingProtectionLevel protectionLevel)
            : displayName(std::move(displayName)), protectionLevel(protectionLevel)
        {
        }

        hstring DisplayName()
        {
            return displayName;
        }

        Windows::Devices::Enumeration::DevicePairingProtectionLevel ProtectionLevel()
        {
            return protectionLevel;
        }

    private:
        hstring displayName;
        Windows::Devices::Enumeration::DevicePairingProtectionLevel protectionLevel;

    };

    struct ProtectionSelectorChoices
    {
        static Windows::Foundation::Collections::IObservableVector<SDKTemplate::ProtectionLevelSelectorInfo> Selectors();
    };

    struct DeviceInformationDisplay : DeviceInformationDisplayT<DeviceInformationDisplay>
    {
    public:
        DeviceInformationDisplay(Windows::Devices::Enumeration::DeviceInformation const& deviceInfoIn);

        Windows::Devices::Enumeration::DeviceInformationKind Kind() const
        {
            return m_deviceInfo.Kind();
        }

        hstring Id() const
        {
            return m_deviceInfo.Id();
        }

        hstring Name() const
        {
            return m_deviceInfo.Name();
        }

        Windows::UI::Xaml::Media::Imaging::BitmapImage GlyphBitmapImage() const
        {
            return m_glyphBitmapImage;
        }

        bool CanPair() const
        {
            return m_deviceInfo.Pairing().CanPair();
        }

        bool IsPaired() const
        {
            return m_deviceInfo.Pairing().IsPaired();
        }

        Windows::Foundation::Collections::IMapView<hstring, IInspectable> Properties()
        {
            return m_deviceInfo.Properties();
        }

        Windows::Devices::Enumeration::DeviceInformation DeviceInformation() const
        {
            return m_deviceInfo;
        }

        void Update(Windows::Devices::Enumeration::DeviceInformationUpdate const& deviceInfoUpdate);
        hstring GetPropertyForDisplay(hstring const& key);
        Windows::Foundation::Collections::IVectorView<hstring> Stuff() { return nullptr; }

        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
        {
            return m_propertyChanged.add(handler);
        }

        void PropertyChanged(event_token const& token)
        {
            m_propertyChanged.remove(token);
        }

    private:
        Windows::Devices::Enumeration::DeviceInformation m_deviceInfo{ nullptr };
        Windows::UI::Xaml::Media::Imaging::BitmapImage m_glyphBitmapImage{ nullptr };
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;

        fire_and_forget UpdateGlyphBitmapImage();
        void OnPropertyChanged(hstring const& name);
    };
}
