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

#include "pch.h"
#include "Scenario2_Client.h"
#include "Scenario2_Client.g.cpp"
#include "SampleConfiguration.h"
#include "BluetoothLEAttributeDisplay.h"

using namespace winrt;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::Security::Cryptography;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

namespace
{
    void SetVisibility(UIElement const& element, bool visible)
    {
        element.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
    }

    // Utility function to convert a string to an int32_t and detect bad input
    bool TryParseInt(const wchar_t* str, int32_t& result)
    {
        wchar_t* end;
        errno = 0;
        result = std::wcstol(str, &end, 0);

        if (str == end)
        {
            // Not parseable.
            return false;
        }

        if (errno == ERANGE || result < INT_MIN || INT_MAX < result)
        {
            // Out of range.
            return false;
        }

        if (*end != L'\0')
        {
            // Extra unparseable characters at the end.
            return false;
        }

        return true;
    }

    template<typename T>
    T Read(DataReader const& reader);

    template<>
    uint32_t Read<uint32_t>(DataReader const& reader)
    {
        return reader.ReadUInt32();
    }

    template<>
    int32_t Read<int32_t>(DataReader const& reader)
    {
        return reader.ReadInt32();
    }

    template<>
    uint8_t Read<uint8_t>(DataReader const& reader)
    {
        return reader.ReadByte();
    }

    template<typename T>
    bool TryExtract(IBuffer const& buffer, T& result)
    {
        if (buffer.Length() >= sizeof(T))
        {
            DataReader reader = DataReader::FromBuffer(buffer);
            result = Read<T>(reader);
            return true;
        }
        return false;
    }
}

namespace winrt
{
    hstring to_hstring(GattCommunicationStatus status)
    {
        switch (status)
        {
        case GattCommunicationStatus::Success: return L"Success";
        case GattCommunicationStatus::Unreachable: return L"Unreachable";
        case GattCommunicationStatus::ProtocolError: return L"ProtocolError";
        case GattCommunicationStatus::AccessDenied: return L"AccessDenied";
        }
        return to_hstring(static_cast<int>(status));
    }
}

namespace winrt::SDKTemplate::implementation
{
#pragma region UI Code
    Scenario2_Client::Scenario2_Client()
    {
        InitializeComponent();
    }

    void Scenario2_Client::OnNavigatedTo(NavigationEventArgs const&)
    {
        SelectedDeviceRun().Text(SampleState::SelectedBleDeviceName);
        if (SampleState::SelectedBleDeviceId.empty())
        {
            ConnectButton().IsEnabled(false);
        }
    }

    fire_and_forget Scenario2_Client::OnNavigatedFrom(NavigationEventArgs const&)
    {
        auto lifetime = get_strong();
        if (!co_await ClearBluetoothLEDeviceAsync())
        {
            rootPage.NotifyUser(L"Error: Unable to reset app state", NotifyType::ErrorMessage);
        }
    }
#pragma endregion

#pragma region Enumerating Services
    IAsyncOperation<bool> Scenario2_Client::ClearBluetoothLEDeviceAsync()
    {
        auto lifetime = get_strong();

        if (notificationsToken)
        {
            // Need to clear the CCCD from the remote device so we stop receiving notifications
            GattCommunicationStatus result = co_await registeredCharacteristic.WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue::None);
            if (result != GattCommunicationStatus::Success)
            {
                co_return false;
            }
            else
            {
                selectedCharacteristic.ValueChanged(std::exchange(notificationsToken, {}));
            }
        }

        if (bluetoothLeDevice != nullptr)
        {
            bluetoothLeDevice.Close();
            bluetoothLeDevice = nullptr;
        }
        co_return true;
    }

    fire_and_forget Scenario2_Client::ConnectButton_Click()
    {
        auto lifetime = get_strong();

        ConnectButton().IsEnabled(false);

        if (!co_await ClearBluetoothLEDeviceAsync())
        {
            rootPage.NotifyUser(L"Error: Unable to reset state, try again.", NotifyType::ErrorMessage);
            ConnectButton().IsEnabled(true);
            co_return;
        }

        try
        {
            // BT_Code: BluetoothLEDevice.FromIdAsync must be called from a UI thread because it may prompt for consent.
            bluetoothLeDevice = co_await BluetoothLEDevice::FromIdAsync(SampleState::SelectedBleDeviceId);

            if (bluetoothLeDevice == nullptr)
            {
                rootPage.NotifyUser(L"Failed to connect to device.", NotifyType::ErrorMessage);
            }
        }
        catch (hresult_error& ex)
        {
            if (ex.to_abi() == HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_AVAILABLE))
            {
                rootPage.NotifyUser(L"Bluetooth radio is not on.", NotifyType::ErrorMessage);
            }
            else
            {
                throw;
            }
        }

        if (bluetoothLeDevice != nullptr)
        {
            // Note: BluetoothLEDevice.GattServices property will return an empty list for unpaired devices. For all uses we recommend using the GetGattServicesAsync method.
            // BT_Code: GetGattServicesAsync returns a list of all the supported services of the device (even if it's not paired to the system).
            // If the services supported by the device are expected to change during BT usage, subscribe to the GattServicesChanged event.
            GattDeviceServicesResult result = co_await bluetoothLeDevice.GetGattServicesAsync(BluetoothCacheMode::Uncached);

            if (result.Status() == GattCommunicationStatus::Success)
            {
                IVectorView<GattDeviceService> services = result.Services();
                rootPage.NotifyUser(L"Found " + to_hstring(services.Size()) + L" services", NotifyType::StatusMessage);
                for (auto&& service : services)
                {
                    ComboBoxItem item;
                    item.Content(box_value(DisplayHelpers::GetServiceName(service)));
                    item.Tag(service);
                    ServiceList().Items().Append(item);
                }
                ConnectButton().Visibility(Visibility::Collapsed);
                ServiceList().Visibility(Visibility::Visible);
            }
            else
            {
                rootPage.NotifyUser(L"Device unreachable", NotifyType::ErrorMessage);
            }
        }
        ConnectButton().IsEnabled(true);
    }

#pragma region Enumerating Characteristics
    fire_and_forget Scenario2_Client::ServiceList_SelectionChanged()
    {
        auto lifetime = get_strong();

        auto selectedItem = ServiceList().SelectedItem().as<ComboBoxItem>();
        GattDeviceService service = selectedItem ? selectedItem.Tag().as<GattDeviceService>() : nullptr;

        CharacteristicList().Items().Clear();
        RemoveValueChangedHandler();

        IVectorView<GattCharacteristic> characteristics{ nullptr };
        try
        {
            // Ensure we have access to the device.
            auto accessStatus = co_await service.RequestAccessAsync();
            if (accessStatus == DeviceAccessStatus::Allowed)
            {
                // BT_Code: Get all the child characteristics of a service. Use the cache mode to specify uncached characterstics only 
                // and the new Async functions to get the characteristics of unpaired devices as well. 
                GattCharacteristicsResult result = co_await service.GetCharacteristicsAsync(BluetoothCacheMode::Uncached);
                if (result.Status() == GattCommunicationStatus::Success)
                {
                    characteristics = result.Characteristics();
                }
                else
                {
                    rootPage.NotifyUser(L"Error accessing service.", NotifyType::ErrorMessage);
                }
            }
            else
            {
                // Not granted access
                rootPage.NotifyUser(L"Error accessing service.", NotifyType::ErrorMessage);

            }
        }
        catch (hresult_error& ex)
        {
            rootPage.NotifyUser(L"Restricted service. Can't read characteristics: " + ex.message(), NotifyType::ErrorMessage);
        }

        if (characteristics)
        {
            for (GattCharacteristic&& c : characteristics)
            {
                ComboBoxItem item;
                item.Content(box_value(DisplayHelpers::GetCharacteristicName(c)));
                item.Tag(c);
                CharacteristicList().Items().Append(item);
            }
        }
        CharacteristicList().Visibility(Visibility::Visible);
    }
#pragma endregion

    void Scenario2_Client::AddValueChangedHandler()
    {
        ValueChangedSubscribeToggle().Content(box_value(L"Unsubscribe from value changes"));
        if (!notificationsToken)
        {
            registeredCharacteristic = selectedCharacteristic;
            notificationsToken = registeredCharacteristic.ValueChanged({ get_weak(), &Scenario2_Client::Characteristic_ValueChanged });
        }
    }
    void Scenario2_Client::RemoveValueChangedHandler()
    {
        ValueChangedSubscribeToggle().Content(box_value(L"Subscribe to value changes"));
        if (notificationsToken)
        {
            registeredCharacteristic.ValueChanged(std::exchange(notificationsToken, {}));
            registeredCharacteristic = nullptr;
        }
    }

    fire_and_forget Scenario2_Client::CharacteristicList_SelectionChanged()
    {
        auto lifetime = get_strong();

        auto selectedItem = ServiceList().SelectedItem().as<ComboBoxItem>();
        selectedCharacteristic = selectedItem ? selectedItem.Tag().as<GattCharacteristic>() : nullptr;

        if (selectedCharacteristic == nullptr)
        {
            EnableCharacteristicPanels(GattCharacteristicProperties::None);
            rootPage.NotifyUser(L"No characteristic selected", NotifyType::ErrorMessage);
            co_return;
        }

        // Get all the child descriptors of a characteristics. Use the cache mode to specify uncached descriptors only 
        // and the new Async functions to get the descriptors of unpaired devices as well. 
        GattDescriptorsResult result = co_await selectedCharacteristic.GetDescriptorsAsync(BluetoothCacheMode::Uncached);
        if (result.Status() != GattCommunicationStatus::Success)
        {
            rootPage.NotifyUser(L"Descriptor read failure: " + to_hstring(result.Status()), NotifyType::ErrorMessage);
        }

        // BT_Code: There's no need to access presentation format unless there's at least one. 
        presentationFormat = nullptr;
        if (selectedCharacteristic.PresentationFormats().Size() > 0)
        {

            if (selectedCharacteristic.PresentationFormats().Size() == 1)
            {
                // Get the presentation format since there's only one way of presenting it
                presentationFormat = selectedCharacteristic.PresentationFormats().GetAt(0);
            }
            else
            {
                // It's difficult to figure out how to split up a characteristic and encode its different parts properly.
                // In this case, we'll just encode the whole thing to a string to make it easy to print out.
            }
        }

        // Enable/disable operations based on the GattCharacteristicProperties.
        EnableCharacteristicPanels(selectedCharacteristic.CharacteristicProperties());
    }

    void Scenario2_Client::EnableCharacteristicPanels(GattCharacteristicProperties properties)
    {
        // BT_Code: Hide the controls which do not apply to this characteristic.
        SetVisibility(CharacteristicReadButton(),
            (properties & GattCharacteristicProperties::Read) != GattCharacteristicProperties::None);

        SetVisibility(CharacteristicWritePanel(),
            (properties & (GattCharacteristicProperties::Write | GattCharacteristicProperties::WriteWithoutResponse)) != GattCharacteristicProperties::None);
        CharacteristicWriteValue().Text(L"");

        SetVisibility(ValueChangedSubscribeToggle(),
            (properties & (GattCharacteristicProperties::Indicate | GattCharacteristicProperties::Notify)) != GattCharacteristicProperties::None);
    }

    fire_and_forget Scenario2_Client::CharacteristicReadButton_Click()
    {
        auto lifetime = get_strong();

        // BT_Code: Read the actual value from the device by using Uncached.
        GattReadResult result = co_await selectedCharacteristic.ReadValueAsync(BluetoothCacheMode::Uncached);
        if (result.Status() == GattCommunicationStatus::Success)
        {
            rootPage.NotifyUser(L"Read result: " + FormatValueByPresentation(result.Value(), presentationFormat), NotifyType::StatusMessage);
        }
        else
        {
            rootPage.NotifyUser(L"Read failed: Status = " + to_hstring(result.Status()), NotifyType::ErrorMessage);
        }
    }

    fire_and_forget Scenario2_Client::CharacteristicWriteButton_Click()
    {
        auto lifetime = get_strong();

        hstring text = CharacteristicWriteValue().Text();
        if (!text.empty())
        {
            IBuffer writeBuffer = CryptographicBuffer::ConvertStringToBinary(CharacteristicWriteValue().Text(), BinaryStringEncoding::Utf8);

            co_await WriteBufferToSelectedCharacteristicAsync(writeBuffer);
        }
        else
        {
            rootPage.NotifyUser(L"No data to write to device", NotifyType::ErrorMessage);
        }
    }

    fire_and_forget Scenario2_Client::CharacteristicWriteButtonInt_Click()
    {
        auto lifetime = get_strong();

        int32_t readValue;
        if (TryParseInt(CharacteristicWriteValue().Text().begin(), readValue))
        {
            DataWriter writer;
            writer.ByteOrder(ByteOrder::LittleEndian);
            writer.WriteInt32(readValue);

            co_await WriteBufferToSelectedCharacteristicAsync(writer.DetachBuffer());
        }
        else
        {
            rootPage.NotifyUser(L"Data to write has to be an int32", NotifyType::ErrorMessage);
        }
    }

    IAsyncOperation<bool> Scenario2_Client::WriteBufferToSelectedCharacteristicAsync(IBuffer buffer)
    {
        auto lifetime = get_strong();

        try
        {
            // BT_Code: Writes the value from the buffer to the characteristic.
            GattWriteResult result = co_await selectedCharacteristic.WriteValueWithResultAsync(buffer);

            if (result.Status() == GattCommunicationStatus::Success)
            {
                rootPage.NotifyUser(L"Successfully wrote value to device", NotifyType::StatusMessage);
                co_return true;
            }
            else
            {
                rootPage.NotifyUser(L"Write failed: Status = " + to_hstring(result.Status()), NotifyType::ErrorMessage);
                co_return false;
            }
        }
        catch (hresult_error& ex)
        {
            if (ex.code() == E_BLUETOOTH_ATT_INVALID_PDU)
            {
                rootPage.NotifyUser(ex.message(), NotifyType::ErrorMessage);
                co_return false;
            }
            if (ex.code() == E_BLUETOOTH_ATT_WRITE_NOT_PERMITTED || ex.code() == E_ACCESSDENIED)
            {
                // This usually happens when a device reports that it support writing, but it actually doesn't.
                rootPage.NotifyUser(ex.message(), NotifyType::ErrorMessage);
                co_return false;
            }
            throw;
        }
    }

    fire_and_forget Scenario2_Client::ValueChangedSubscribeToggle_Click()
    {
        auto lifetime = get_strong();

        if (!notificationsToken)
        {
            GattClientCharacteristicConfigurationDescriptorValue cccdValue = GattClientCharacteristicConfigurationDescriptorValue::None;
            if ((selectedCharacteristic.CharacteristicProperties() & GattCharacteristicProperties::Indicate) != GattCharacteristicProperties::None)
            {
                cccdValue = GattClientCharacteristicConfigurationDescriptorValue::Indicate;
            }

            else if ((selectedCharacteristic.CharacteristicProperties() & GattCharacteristicProperties::Notify) != GattCharacteristicProperties::None)
            {
                cccdValue = GattClientCharacteristicConfigurationDescriptorValue::Notify;
            }

            try
            {
                // BT_Code: Must write the CCCD in order for server to send indications.
                // We receive them in the ValueChanged event handler.
                GattCommunicationStatus status = co_await selectedCharacteristic.WriteClientCharacteristicConfigurationDescriptorAsync(cccdValue);

                if (status == GattCommunicationStatus::Success)
                {
                    AddValueChangedHandler();
                    rootPage.NotifyUser(L"Successfully subscribed for value changes", NotifyType::StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser(L"Error registering for value changes: Status = " + to_hstring(status), NotifyType::ErrorMessage);
                }
            }
            catch (hresult_access_denied& ex)
            {
                // This usually happens when a device reports that it support indicate, but it actually doesn't.
                rootPage.NotifyUser(ex.message(), NotifyType::ErrorMessage);
            }
        }
        else
        {
            try
            {
                // BT_Code: Must write the CCCD in order for server to send notifications.
                // We receive them in the ValueChanged event handler.
                // Note that this sample configures either Indicate or Notify, but not both.
                GattCommunicationStatus result = co_await
                    selectedCharacteristic.WriteClientCharacteristicConfigurationDescriptorAsync(
                        GattClientCharacteristicConfigurationDescriptorValue::None);
                if (result == GattCommunicationStatus::Success)
                {
                    RemoveValueChangedHandler();
                    rootPage.NotifyUser(L"Successfully un-registered for notifications", NotifyType::StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser(L"Error un-registering for notifications: Status = " + to_hstring(result), NotifyType::ErrorMessage);
                }
            }
            catch (hresult_access_denied& ex)
            {
                // This usually happens when a device reports that it support notify, but it actually doesn't.
                rootPage.NotifyUser(ex.message(), NotifyType::ErrorMessage);
            }
        }
    }

    fire_and_forget Scenario2_Client::Characteristic_ValueChanged(GattCharacteristic const&, GattValueChangedEventArgs args)
    {
        auto lifetime = get_strong();

        // BT_Code: An Indicate or Notify reported that the value has changed.
        // Display the new value with a timestamp.
        hstring newValue = FormatValueByPresentation(args.CharacteristicValue(), presentationFormat);
        std::time_t now = clock::to_time_t(clock::now());
        char buffer[26];
        ctime_s(buffer, ARRAYSIZE(buffer), &now);
        hstring message = L"Value at " + to_hstring(buffer) + L": " + newValue;
        co_await resume_foreground(Dispatcher());
        CharacteristicLatestValue().Text(message);
    }

    /// <summary>
    /// Process the raw data received from the device into application usable data,
    /// according the the Bluetooth Heart Rate Profile.
    /// https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.heart_rate_measurement.xml&u=org.bluetooth.characteristic.heart_rate_measurement.xml
    /// This function throws an std::out_of_range if the data cannot be parsed.
    /// </summary>
    /// <param name="data">Raw data received from the heart rate monitor.</param>
    /// <returns>The heart rate measurement value.</returns>
    int32_t ParseHeartRateValue(IBuffer const& buffer)
    {
        // Heart Rate profile defined flag values
        const uint8_t heartRateValueFormat = 0x1;

        com_array<uint8_t> bytes;
        CryptographicBuffer::CopyToByteArray(buffer, bytes);
        
        if (bytes.at(0) & heartRateValueFormat)
        {
            return bytes.at(1) | (bytes.at(2) << 8);
        }
        return bytes.at(1);
    }

    hstring Scenario2_Client::FormatValueByPresentation(IBuffer const& buffer, GenericAttributeProfile::GattPresentationFormat const& format)
    {
        // BT_Code: For the purpose of this sample, this function converts only UInt32 and
        // UTF-8 buffers to readable text. It can be extended to support other formats if your app needs them.
        if (format != nullptr)
        {
            if (format.FormatType() == GattPresentationFormatTypes::UInt32())
            {
                uint32_t value;
                if (TryExtract(buffer, value))
                {
                    return to_hstring(value);
                }
                return L"(error: Invalid UInt32)";

            }
            else if (format.FormatType() == GattPresentationFormatTypes::Utf8())
            {
                try
                {
                    return CryptographicBuffer::ConvertBinaryToString(BinaryStringEncoding::Utf8, buffer);
                }
                catch (hresult_invalid_argument&)
                {
                    return L"(error: Invalid UTF-8 string)";
                }
            }
            else
            {
                // Add support for other format types as needed.
                return L"Unsupported format: " + CryptographicBuffer::EncodeToHexString(buffer);
            }
        }
        else if (buffer != nullptr)
        {
            // We don't know what format to use. Let's try some well-known profiles, or default back to UTF-8.
            if (selectedCharacteristic.Uuid() == GattCharacteristicUuids::HeartRateMeasurement())
            {
                try
                {
                    return L"Heart Rate: " + to_hstring(ParseHeartRateValue(buffer));
                }
                catch (std::out_of_range&)
                {
                    return L"Heart Rate: (unable to parse)";
                }
            }
            else if (selectedCharacteristic.Uuid() == GattCharacteristicUuids::BatteryLevel())
            {
                // battery level is encoded as a percentage value in the first byte according to
                // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.battery_level.xml
                uint8_t percent;
                if (TryExtract(buffer, percent))
                {
                    return L"Battery Level: " + to_hstring(percent) + L"%";
                }
                return L"Battery Level: (unable to parse)";
            }
            // This is our custom calc service Result UUID. Format it like an Int
            else if (selectedCharacteristic.Uuid() == Constants::ResultCharacteristicUuid)
            {
                int32_t value;
                if (TryExtract(buffer, value))
                {
                    return to_hstring(value);
                }
                return L"Invalid response from calc server";
            }
            // No guarantees on if a characteristic is registered for notifications.
            else if (registeredCharacteristic != nullptr)
            {
                // This is our custom calc service Result UUID. Format it like an Int
                if (registeredCharacteristic.Uuid() == Constants::ResultCharacteristicUuid)
                {
                    int32_t value;
                    if (TryExtract(buffer, value))
                    {
                        return to_hstring(value);
                    }
                    return L"Invalid response from calc server";
                }
            }
            else
            {
                try
                {
                    return L"Unknown format: " + CryptographicBuffer::ConvertBinaryToString(BinaryStringEncoding::Utf8, buffer);

                }
                catch (hresult_invalid_argument&)
                {
                    return L"Unknown format";
                }
            }
        }
        else
        {
            return L"Empty data received";
        }
        return L"Unknown format";
    }
}
