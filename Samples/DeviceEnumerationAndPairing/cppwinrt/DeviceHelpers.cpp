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
#include "DeviceHelpers.h"

// NOTE: The following header files are only needed in order to demonstrate many of the
// different device selectors available from Windows Runtime APIs. You will only need to include
// the ones for the Windows Runtime API your actual scenario needs.
#include "winrt/Windows.Devices.HumanInterfaceDevice.h"
#include "winrt/Windows.Networking.Proximity.h"
#include "winrt/Windows.Devices.Bluetooth.h"
#include "winrt/Windows.Devices.WiFiDirect.h"
#include "winrt/Windows.Media.Casting.h"
#include "winrt/Windows.Media.DialProtocol.h"
#include "winrt/Windows.Devices.Sensors.h"
#include "winrt/Windows.Devices.PointOfService.h"

using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml::Media::Imaging;
using namespace winrt::Windows::UI::Xaml::Data;

namespace winrt::SDKTemplate::implementation
{
    IObservableVector<SDKTemplate::DeviceSelectorInfo> DeviceSelectorChoices::CommonDeviceSelectors()
    {
        return single_threaded_observable_vector<SDKTemplate::DeviceSelectorInfo>({
            // Pre-canned device class selectors
            make<DeviceSelectorInfo>(L"All Device Interfaces (default)", DeviceClass::All),
            make<DeviceSelectorInfo>(L"Audio Capture", DeviceClass::AudioCapture),
            make<DeviceSelectorInfo>(L"Audio Render", DeviceClass::AudioRender),
            make<DeviceSelectorInfo>(L"Image Scanner", DeviceClass::ImageScanner),
            make<DeviceSelectorInfo>(L"Location", DeviceClass::Location),
            make<DeviceSelectorInfo>(L"Portable Storage", DeviceClass::PortableStorageDevice),
            make<DeviceSelectorInfo>(L"Video Capture", DeviceClass::VideoCapture),

            // A few examples of selectors built dynamically by windows runtime apis.
            make<DeviceSelectorInfo>(L"Human Interface (HID)", Windows::Devices::HumanInterfaceDevice::HidDevice::GetDeviceSelector(0, 0)),
            make<DeviceSelectorInfo>(L"Activity Sensor", Windows::Devices::Sensors::ActivitySensor::GetDeviceSelector()),
            make<DeviceSelectorInfo>(L"Pedometer", Windows::Devices::Sensors::Pedometer::GetDeviceSelector()),
            make<DeviceSelectorInfo>(L"Proximity", Windows::Networking::Proximity::ProximityDevice::GetDeviceSelector()),
            make<DeviceSelectorInfo>(L"Proximity Sensor", Windows::Devices::Sensors::ProximitySensor::GetDeviceSelector()), });
    }

    namespace
    {
        SDKTemplate::DeviceSelectorInfo Bluetooth()
        {
            // Currently Bluetooth APIs don't provide a selector to get ALL devices that are both paired and non-paired.  Typically you wouldn't need this for common scenarios, but it's convenient to demonstrate the
            // various sample scenarios.
            return make<DeviceSelectorInfo>(L"Bluetooth", L"System.Devices.Aep.ProtocolId:=\"{e0cbf06c-cd8b-4647-bb8a-263b43f0f974}\"", DeviceInformationKind::AssociationEndpoint);
        }

        SDKTemplate::DeviceSelectorInfo BluetoothUnpairedOnly()
        {
            return make<DeviceSelectorInfo>(L"Bluetooth (unpaired)", Windows::Devices::Bluetooth::BluetoothDevice::GetDeviceSelectorFromPairingState(false));
        }

        SDKTemplate::DeviceSelectorInfo BluetoothPairedOnly()
        {
            return make<DeviceSelectorInfo>(L"Bluetooth (paired)", Windows::Devices::Bluetooth::BluetoothDevice::GetDeviceSelectorFromPairingState(true));
        }

        SDKTemplate::DeviceSelectorInfo BluetoothLE()
        {
            // Currently Bluetooth APIs don't provide a selector to get ALL devices that are both paired and non-paired.  Typically you wouldn't need this for common scenarios, but it's convenient to demonstrate the
            // various sample scenarios.
            return make<DeviceSelectorInfo>(L"Bluetooth LE", L"System.Devices.Aep.ProtocolId:=\"{bb7bb05e-5972-42b5-94fc-76eaa7084d49}\"", DeviceInformationKind::AssociationEndpoint);
        }

        SDKTemplate::DeviceSelectorInfo BluetoothLEUnpairedOnly()
        {
            return make<DeviceSelectorInfo>(L"Bluetooth LE (unpaired)", Windows::Devices::Bluetooth::BluetoothLEDevice::GetDeviceSelectorFromPairingState(false));
        }

        SDKTemplate::DeviceSelectorInfo BluetoothLEPairedOnly()
        {
            return make<DeviceSelectorInfo>(L"Bluetooth LE (paired)", Windows::Devices::Bluetooth::BluetoothLEDevice::GetDeviceSelectorFromPairingState(true));
        }

        SDKTemplate::DeviceSelectorInfo WiFiDirect()
        {
            return make<DeviceSelectorInfo>(L"Wi-Fi Direct", Windows::Devices::WiFiDirect::WiFiDirectDevice::GetDeviceSelector(Windows::Devices::WiFiDirect::WiFiDirectDeviceSelectorType::AssociationEndpoint));
        }

        SDKTemplate::DeviceSelectorInfo WiFiDirectPairedOnly()
        {
            return make<DeviceSelectorInfo>(L"Wi-Fi Direct (paired)", Windows::Devices::WiFiDirect::WiFiDirectDevice::GetDeviceSelector());
        }

        SDKTemplate::DeviceSelectorInfo PointOfServicePrinter()
        {
            return make<DeviceSelectorInfo>(L"Point of Service Printer", Windows::Devices::PointOfService::PosPrinter::GetDeviceSelector());
        }

        SDKTemplate::DeviceSelectorInfo VideoCasting()
        {
            return make<DeviceSelectorInfo>(L"Video Casting", Windows::Media::Casting::CastingDevice::GetDeviceSelector(Windows::Media::Casting::CastingPlaybackTypes::Video));
        }

        SDKTemplate::DeviceSelectorInfo DialAllApps()
        {
            return make<DeviceSelectorInfo>(L"DIAL (All apps)", Windows::Media::DialProtocol::DialDevice::GetDeviceSelector(L""));
        }

        // WSD and UPnP are unique in that there are currently no general WSD or UPnP APIs to USE the devices once you've discovered them.
        // You can pair the devices using DeviceInformation.Pairing.PairAsync etc, and you can USE them with the sockets APIs. However, since
        // there's no specific API right now, there's no *.GetDeviceSelector available.  That's why we just simply build the device selector
        // ourselves and specify the correct DeviceInformationKind (AssociationEndpoint).
        SDKTemplate::DeviceSelectorInfo Wsd()
        {
            return make<DeviceSelectorInfo>(L"Web Services on Devices (WSD)", L"System.Devices.Aep.ProtocolId:=\"{782232aa-a2f9-4993-971b-aedc551346b0}\"", DeviceInformationKind::AssociationEndpoint);
        }

        SDKTemplate::DeviceSelectorInfo Upnp()
        {
            return make<DeviceSelectorInfo>(L"UPnP", L"System.Devices.Aep.ProtocolId:=\"{0e261de4-12f0-46e6-91ba-428607ccef64}\"", DeviceInformationKind::AssociationEndpoint);
        }

        SDKTemplate::DeviceSelectorInfo NetworkCamera()
        {
            return make<DeviceSelectorInfo>(L"Web Services on Devices (NetworkCamera)", L"System.Devices.Aep.ProtocolId:=\"{43cc0de4-8ca8-4a56-805a-86fc63f21602}\"", DeviceInformationKind::AssociationEndpoint);
        }

        void AddVideoCastingIfSupported(IObservableVector<SDKTemplate::DeviceSelectorInfo>& selectors)
        {
            try
            {
                selectors.Append(VideoCasting());
            }
            catch (hresult_error const& ex)
            {

                if (ex.code() == HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
                {
                    // Video casting is not supported by the system.
                }
                else
                {
                    throw;
                }
            }
        }
    }

    IObservableVector<SDKTemplate::DeviceSelectorInfo> DeviceSelectorChoices::DevicePickerSelectors()
    {
        // Add all selectors that can be used with the DevicePicker
        IObservableVector<SDKTemplate::DeviceSelectorInfo> selectors = CommonDeviceSelectors();
        selectors.Append(BluetoothPairedOnly());
        selectors.Append(BluetoothUnpairedOnly());
        selectors.Append(BluetoothLEPairedOnly());
        selectors.Append(BluetoothLEUnpairedOnly());
        selectors.Append(WiFiDirect());
        selectors.Append(PointOfServicePrinter());
        AddVideoCastingIfSupported(selectors);
        selectors.Append(DialAllApps());

        return selectors;
    }

    IObservableVector<SDKTemplate::DeviceSelectorInfo> DeviceSelectorChoices::FindAllAsyncSelectors()
    {
        IObservableVector<SDKTemplate::DeviceSelectorInfo> selectors = CommonDeviceSelectors();

        selectors.Append(BluetoothPairedOnly());
        selectors.Append(BluetoothLEPairedOnly());
        selectors.Append(WiFiDirectPairedOnly());

        return selectors;
    }

    IObservableVector<SDKTemplate::DeviceSelectorInfo> DeviceSelectorChoices::DeviceWatcherSelectors()
    {
        IObservableVector<SDKTemplate::DeviceSelectorInfo> selectors = CommonDeviceSelectors();

        selectors.Append(Bluetooth());
        selectors.Append(BluetoothLE());
        selectors.Append(WiFiDirect());
        selectors.Append(PointOfServicePrinter());
        AddVideoCastingIfSupported(selectors);
        selectors.Append(DialAllApps());
        selectors.Append(Wsd());
        selectors.Append(Upnp());

        return selectors;
    }

    IObservableVector<SDKTemplate::DeviceSelectorInfo> DeviceSelectorChoices::BackgroundDeviceWatcherSelectors()
    {
        IObservableVector<SDKTemplate::DeviceSelectorInfo> selectors = CommonDeviceSelectors();

        selectors.Append(BluetoothPairedOnly());
        selectors.Append(BluetoothLEPairedOnly());
        selectors.Append(WiFiDirectPairedOnly());
        selectors.Append(PointOfServicePrinter());
        AddVideoCastingIfSupported(selectors);
        selectors.Append(DialAllApps());
        selectors.Append(Wsd());
        selectors.Append(Upnp());

        return selectors;
    }

    IObservableVector<SDKTemplate::DeviceSelectorInfo> DeviceSelectorChoices::PairingSelectors()
    {
        IObservableVector<SDKTemplate::DeviceSelectorInfo> selectors = single_threaded_observable_vector<SDKTemplate::DeviceSelectorInfo>();

        selectors.Append(Bluetooth());
        selectors.Append(BluetoothLE());
        selectors.Append(WiFiDirect());
        selectors.Append(PointOfServicePrinter());
        AddVideoCastingIfSupported(selectors);
        selectors.Append(Wsd());
        selectors.Append(Upnp());
        selectors.Append(NetworkCamera());

        return selectors;
    }

    IObservableVector<SDKTemplate::DeviceInformationKindChoice> DeviceInformationKindChoices::Choices()
    {
        return single_threaded_observable_vector<SDKTemplate::DeviceInformationKindChoice>({
            make<DeviceInformationKindChoice>(L"DeviceContainer", std::vector{ DeviceInformationKind::DeviceContainer }),
            make<DeviceInformationKindChoice>(L"Device", std::vector{ DeviceInformationKind::Device }),
            make<DeviceInformationKindChoice>(L"DeviceInterface (Default)", std::vector{ DeviceInformationKind::DeviceInterface }),
            make<DeviceInformationKindChoice>(L"DeviceInterfaceClass", std::vector{ DeviceInformationKind::DeviceInterfaceClass }),
            make<DeviceInformationKindChoice>(L"AssociationEndpointContainer", std::vector{ DeviceInformationKind::AssociationEndpointContainer }),
            make<DeviceInformationKindChoice>(L"AssociationEndpoint", std::vector{ DeviceInformationKind::AssociationEndpoint }),
            make<DeviceInformationKindChoice>(L"AssociationEndpointService", std::vector{ DeviceInformationKind::AssociationEndpointService }),
            make<DeviceInformationKindChoice>(L"AssociationEndpointService and DeviceInterface", std::vector{ DeviceInformationKind::AssociationEndpointService, DeviceInformationKind::DeviceInterface }),
        });
    }

    IObservableVector<SDKTemplate::ProtectionLevelSelectorInfo> ProtectionSelectorChoices::Selectors()
    {
        return single_threaded_observable_vector<SDKTemplate::ProtectionLevelSelectorInfo>({
            make<ProtectionLevelSelectorInfo>(L"Default", DevicePairingProtectionLevel::Default),
            make<ProtectionLevelSelectorInfo>(L"None", DevicePairingProtectionLevel::None),
            make<ProtectionLevelSelectorInfo>(L"Encryption", DevicePairingProtectionLevel::Encryption),
            make<ProtectionLevelSelectorInfo>(L"Encryption and authentication", DevicePairingProtectionLevel::EncryptionAndAuthentication),
            });
    }


    DeviceInformationDisplay::DeviceInformationDisplay(::DeviceInformation const& deviceInfoIn)
        : m_deviceInfo(deviceInfoIn)
    {
        UpdateGlyphBitmapImage();
    }

    void DeviceInformationDisplay::Update(DeviceInformationUpdate const& deviceInfoUpdate)
    {
        m_deviceInfo.Update(deviceInfoUpdate);

        OnPropertyChanged(L"Kind");
        OnPropertyChanged(L"Id");
        OnPropertyChanged(L"Name");
        OnPropertyChanged(L"DeviceInformation");
        OnPropertyChanged(L"CanPair");
        OnPropertyChanged(L"IsPaired");
        OnPropertyChanged(L"GetPropertyForDisplay");

        UpdateGlyphBitmapImage();
    }

    hstring DeviceInformationDisplay::GetPropertyForDisplay(hstring const& key)
    {
        auto value = Properties().TryLookup(key);
        if (auto v = value.try_as<IReference<guid>>(); v)
        {
            return to_hstring(v.Value());
        }

        return unbox_value_or<hstring>(value, {});
    }

    fire_and_forget DeviceInformationDisplay::UpdateGlyphBitmapImage()
    {
        auto lifetime = get_strong();

        DeviceThumbnail deviceThumbnail = co_await m_deviceInfo.GetGlyphThumbnailAsync();
        BitmapImage glyphBitmapImage;
        co_await glyphBitmapImage.SetSourceAsync(deviceThumbnail);
        m_glyphBitmapImage = glyphBitmapImage;
        OnPropertyChanged(L"GlyphBitmapImage");
    }

    void DeviceInformationDisplay::OnPropertyChanged(hstring const& name)
    {
        m_propertyChanged(*this, PropertyChangedEventArgs(name));
    }
}