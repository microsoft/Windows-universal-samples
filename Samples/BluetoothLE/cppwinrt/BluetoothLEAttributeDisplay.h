#pragma once

#include "BluetoothLEAttributeDisplay.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct BluetoothLEAttributeDisplay : BluetoothLEAttributeDisplayT<BluetoothLEAttributeDisplay>
    {
        BluetoothLEAttributeDisplay(Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService const& service)
            : m_service(service), m_attributeType(SDKTemplate::AttributeType::Service)
        {
        }

        BluetoothLEAttributeDisplay(Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic const& characteristic)
            : m_characteristic(characteristic), m_attributeType(SDKTemplate::AttributeType::Characteristic)
        {
        }

        Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic characteristic()
        {
            return m_characteristic;
        }

        void characteristic(Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic const& value)
        {
            m_characteristic = value;
        }

        Windows::Devices::Bluetooth::GenericAttributeProfile::GattDescriptor descriptor()
        {
            return m_descriptor;
        }

        void descriptor(Windows::Devices::Bluetooth::GenericAttributeProfile::GattDescriptor const& value)
        {
            m_descriptor = value;
        }

        Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService service()
        {
            return m_service;
        }

        void service(Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService const& value)
        {
            m_service = value;
        }

        hstring Name();
        hstring AttributeDisplayType();

        static SDKTemplate::BluetoothLEAttributeDisplay CreateFromService(Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService const& service)
        {
            return BluetoothLEAttributeDisplay(service);
        }

        static SDKTemplate::BluetoothLEAttributeDisplay CreateFromCharacteristic(Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic const& characteristic)
        {
            return BluetoothLEAttributeDisplay(characteristic);
        }

    private:
        Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic m_characteristic{ nullptr };
        Windows::Devices::Bluetooth::GenericAttributeProfile::GattDescriptor m_descriptor{ nullptr };
        Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService m_service{ nullptr };
        SDKTemplate::AttributeType m_attributeType;
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct BluetoothLEAttributeDisplay : BluetoothLEAttributeDisplayT<BluetoothLEAttributeDisplay, implementation::BluetoothLEAttributeDisplay>
    {
    };
}
