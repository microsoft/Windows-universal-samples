#include "pch.h"
#include "BluetoothLEAttributeDisplay.h"

using namespace winrt;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;

namespace
{
    /// <summary>
    ///  Determines whether the UUID was assigned by the Bluetooth SIG.
    ///  If so, extracts the assigned number.
    /// </summary>

    bool TryParseSigDefinedUuid(guid const& uuid, uint16_t& shortId)
    {
        // UUIDs defined by the Bluetooth SIG are of the form
        // 0000xxxx-0000-1000-8000-00805F9B34FB.
        constexpr guid BluetoothGuid = { 0x00000000, 0x0000, 0x1000, { 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB } };

        shortId = static_cast<uint16_t>(uuid.Data1);
        guid possibleBluetoothGuid = uuid;
        possibleBluetoothGuid.Data1 &= 0xFFFF0000;
        return possibleBluetoothGuid == BluetoothGuid;
    }
}

namespace winrt::SDKTemplate::DisplayHelpers
{
    hstring GetServiceName(GattDeviceService const& service)
    {
        uint16_t shortId;

        guid uuid = service.Uuid();
        if (TryParseSigDefinedUuid(uuid, shortId))
        {
            // Reference: https://developer.bluetooth.org/gatt/services/Pages/ServicesHome.aspx
            const static std::map<uint32_t, const wchar_t*> knownServiceIds =
            {
                { 0x0000, L"None" },
                { 0x1811, L"AlertNotification" },
                { 0x180F, L"Battery" },
                { 0x1810, L"BloodPressure" },
                { 0x1805, L"CurrentTimeService" },
                { 0x1816, L"CyclingSpeedandCadence" },
                { 0x180A, L"DeviceInformation" },
                { 0x1800, L"GenericAccess" },
                { 0x1801, L"GenericAttribute" },
                { 0x1808, L"Glucose" },
                { 0x1809, L"HealthThermometer" },
                { 0x180D, L"HeartRate" },
                { 0x1812, L"HumanInterfaceDevice" },
                { 0x1802, L"ImmediateAlert" },
                { 0x1803, L"LinkLoss" },
                { 0x1807, L"NextDSTChange" },
                { 0x180E, L"PhoneAlertStatus" },
                { 0x1806, L"ReferenceTimeUpdateService" },
                { 0x1814, L"RunningSpeedandCadence" },
                { 0x1813, L"ScanParameters" },
                { 0x1804, L"TxPower" },
                { 0xFFE0, L"SimpleKeyService" },
            };
            auto it = knownServiceIds.find(shortId);
            if (it != knownServiceIds.end())
            {
                return it->second;
            }
        }
        return L"Custom service: " + to_hstring(uuid);
    }

    hstring GetCharacteristicName(GattCharacteristic const& characteristic)
    {
        uint16_t shortId;

        guid uuid = characteristic.Uuid();
        if (TryParseSigDefinedUuid(uuid, shortId))
        {
            // Reference: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicsHome.aspx
            const static std::map<uint32_t, const wchar_t*> knownCharacteristicIds =
            {
                { 0x0000, L"None" },
                { 0x2A43, L"AlertCategoryID" },
                { 0x2A42, L"AlertCategoryIDBitMask" },
                { 0x2A06, L"AlertLevel" },
                { 0x2A44, L"AlertNotificationControlPoint" },
                { 0x2A3F, L"AlertStatus" },
                { 0x2A01, L"Appearance" },
                { 0x2A19, L"BatteryLevel" },
                { 0x2A49, L"BloodPressureFeature" },
                { 0x2A35, L"BloodPressureMeasurement" },
                { 0x2A38, L"BodySensorLocation" },
                { 0x2A22, L"BootKeyboardInputReport" },
                { 0x2A32, L"BootKeyboardOutputReport" },
                { 0x2A33, L"BootMouseInputReport" },
                { 0x2A5C, L"CSCFeature" },
                { 0x2A5B, L"CSCMeasurement" },
                { 0x2A2B, L"CurrentTime" },
                { 0x2A08, L"DateTime" },
                { 0x2A0A, L"DayDateTime" },
                { 0x2A09, L"DayofWeek" },
                { 0x2A00, L"DeviceName" },
                { 0x2A0D, L"DSTOffset" },
                { 0x2A0C, L"ExactTime256" },
                { 0x2A26, L"FirmwareRevisionString" },
                { 0x2A51, L"GlucoseFeature" },
                { 0x2A18, L"GlucoseMeasurement" },
                { 0x2A34, L"GlucoseMeasurementContext" },
                { 0x2A27, L"HardwareRevisionString" },
                { 0x2A39, L"HeartRateControlPoint" },
                { 0x2A37, L"HeartRateMeasurement" },
                { 0x2A4C, L"HIDControlPoint" },
                { 0x2A4A, L"HIDInformation" },
                { 0x2A2A, L"IEEE11073_20601RegulatoryCertificationDataList" },
                { 0x2A36, L"IntermediateCuffPressure" },
                { 0x2A1E, L"IntermediateTemperature" },
                { 0x2A0F, L"LocalTimeInformation" },
                { 0x2A29, L"ManufacturerNameString" },
                { 0x2A21, L"MeasurementInterval" },
                { 0x2A24, L"ModelNumberString" },
                { 0x2A46, L"NewAlert" },
                { 0x2A04, L"PeripheralPreferredConnectionParameters" },
                { 0x2A02, L"PeripheralPrivacyFlag" },
                { 0x2A50, L"PnPID" },
                { 0x2A4E, L"ProtocolMode" },
                { 0x2A03, L"ReconnectionAddress" },
                { 0x2A52, L"RecordAccessControlPoint" },
                { 0x2A14, L"ReferenceTimeInformation" },
                { 0x2A4D, L"Report" },
                { 0x2A4B, L"ReportMap" },
                { 0x2A40, L"RingerControlPoint" },
                { 0x2A41, L"RingerSetting" },
                { 0x2A54, L"RSCFeature" },
                { 0x2A53, L"RSCMeasurement" },
                { 0x2A55, L"SCControlPoint" },
                { 0x2A4F, L"ScanIntervalWindow" },
                { 0x2A31, L"ScanRefresh" },
                { 0x2A5D, L"SensorLocation" },
                { 0x2A25, L"SerialNumberString" },
                { 0x2A05, L"ServiceChanged" },
                { 0x2A28, L"SoftwareRevisionString" },
                { 0x2A47, L"SupportedNewAlertCategory" },
                { 0x2A48, L"SupportedUnreadAlertCategory" },
                { 0x2A23, L"SystemID" },
                { 0x2A1C, L"TemperatureMeasurement" },
                { 0x2A1D, L"TemperatureType" },
                { 0x2A12, L"TimeAccuracy" },
                { 0x2A13, L"TimeSource" },
                { 0x2A16, L"TimeUpdateControlPoint" },
                { 0x2A17, L"TimeUpdateState" },
                { 0x2A11, L"TimewithDST" },
                { 0x2A0E, L"TimeZone" },
                { 0x2A07, L"TxPowerLevel" },
                { 0x2A45, L"UnreadAlertStatus" },
                { 0x2A5A, L"AggregateInput" },
                { 0x2A58, L"AnalogInput" },
                { 0x2A59, L"AnalogOutput" },
                { 0x2A66, L"CyclingPowerControlPoint" },
                { 0x2A65, L"CyclingPowerFeature" },
                { 0x2A63, L"CyclingPowerMeasurement" },
                { 0x2A64, L"CyclingPowerVector" },
                { 0x2A56, L"DigitalInput" },
                { 0x2A57, L"DigitalOutput" },
                { 0x2A0B, L"ExactTime100" },
                { 0x2A6B, L"LNControlPoint" },
                { 0x2A6A, L"LNFeature" },
                { 0x2A67, L"LocationandSpeed" },
                { 0x2A68, L"Navigation" },
                { 0x2A3E, L"NetworkAvailability" },
                { 0x2A69, L"PositionQuality" },
                { 0x2A3C, L"ScientificTemperatureinCelsius" },
                { 0x2A10, L"SecondaryTimeZone" },
                { 0x2A3D, L"String" },
                { 0x2A1F, L"TemperatureinCelsius" },
                { 0x2A20, L"TemperatureinFahrenheit" },
                { 0x2A15, L"TimeBroadcast" },
                { 0x2A1B, L"BatteryLevelState" },
                { 0x2A1A, L"BatteryPowerState" },
                { 0x2A5F, L"PulseOximetryContinuousMeasurement" },
                { 0x2A62, L"PulseOximetryControlPoint" },
                { 0x2A61, L"PulseOximetryFeatures" },
                { 0x2A60, L"PulseOximetryPulsatileEvent" },
                { 0xFFE1, L"SimpleKeyState" },
            };

            auto it = knownCharacteristicIds.find(shortId);
            if (it != knownCharacteristicIds.end())
            {
                return it->second;
            }
        }

        hstring userDescription = characteristic.UserDescription();
        if (!userDescription.empty())
        {
            return userDescription;
        }

        return L"Custom Characteristic: " + to_hstring(uuid);
    }
}
