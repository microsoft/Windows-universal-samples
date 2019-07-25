#pragma once

namespace winrt::SDKTemplate::DisplayHelpers
{
    hstring GetServiceName(Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService const& service);
    hstring GetCharacteristicName(Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic const& characteristic);
}
