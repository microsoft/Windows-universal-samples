// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include <ppltasks.h>
#include <DisplayHelpers.h>

using namespace SDKTemplate;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::Devices::Enumeration;

// NOTE: The following using statements are only needed in order to demonstrate many of the
// different device selectors available from Windows Runtime APIs. You will only need to include
// the namespace for the Windows Runtime API your actual scenario needs.
using namespace Windows::Devices::HumanInterfaceDevice;
using namespace Windows::Networking::Proximity;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::WiFiDirect;
using namespace Windows::Media::Casting;
using namespace Windows::Media::DialProtocol;
using namespace Windows::Devices::Sensors;
using namespace Windows::Devices::PointOfService;

namespace SDKTemplate
{
    // DeviceSelectorChoices

    Windows::Foundation::Collections::IVectorView<DeviceSelectorInfo^>^ DeviceSelectorChoices::CommonDeviceSelectors::get()
    {
        Vector<DeviceSelectorInfo^>^ selectors = ref new Vector<DeviceSelectorInfo^>();

        // Pre-canned device class selectors
        selectors->Append(ref new DeviceSelectorInfo("All Device Interfaces (default)", DeviceClass::All, nullptr));
        selectors->Append(ref new DeviceSelectorInfo("Audio Capture", DeviceClass::AudioCapture, nullptr));
        selectors->Append(ref new DeviceSelectorInfo("Audio Render", DeviceClass::AudioRender, nullptr));
        selectors->Append(ref new DeviceSelectorInfo("Image Scanner", DeviceClass::ImageScanner, nullptr));
        selectors->Append(ref new DeviceSelectorInfo("Location", DeviceClass::Location, nullptr));
        selectors->Append(ref new DeviceSelectorInfo("Portable Storage", DeviceClass::PortableStorageDevice, nullptr));
        selectors->Append(ref new DeviceSelectorInfo("Video Capture", DeviceClass::VideoCapture, nullptr));

        // A few examples of selectors built dynamically by windows runtime apis.
        selectors->Append(ref new DeviceSelectorInfo("Human Interface (HID)", DeviceClass::All, HidDevice::GetDeviceSelector(0, 0)));
        selectors->Append(ref new DeviceSelectorInfo("Activity Sensor", DeviceClass::All, ActivitySensor::GetDeviceSelector()));
        selectors->Append(ref new DeviceSelectorInfo("Pedometer", DeviceClass::All, Pedometer::GetDeviceSelector()));
        selectors->Append(ref new DeviceSelectorInfo("Proximity", DeviceClass::All, ProximityDevice::GetDeviceSelector()));
        selectors->Append(ref new DeviceSelectorInfo("Proximity Sensor", DeviceClass::All, ProximityDevice::GetDeviceSelector()));

        return selectors->GetView();
    }

    DeviceSelectorInfo^ DeviceSelectorChoices::Bluetooth::get()
    {
        // Currently Bluetooth APIs don't provide a selector to get ALL devices that are both paired and non-paired.  Typically you wouldn't need this for common scenarios, but it's convenient to demonstrate the
        // various sample scenarios. 
        return ref new DeviceSelectorInfo("Bluetooth", DeviceClass::All, "System.Devices.Aep.ProtocolId:=\"{e0cbf06c-cd8b-4647-bb8a-263b43f0f974}\"", DeviceInformationKind::AssociationEndpoint);
    }

    DeviceSelectorInfo^ DeviceSelectorChoices::BluetoothUnpairedOnly::get()
    {
        return ref new DeviceSelectorInfo("Bluetooth (unpaired)", DeviceClass::All, BluetoothDevice::GetDeviceSelectorFromPairingState(false));
    }

    DeviceSelectorInfo^ DeviceSelectorChoices::BluetoothPairedOnly::get()
    {
        return ref new DeviceSelectorInfo("Bluetooth (paired)", DeviceClass::All, BluetoothDevice::GetDeviceSelectorFromPairingState(true));
    }

    DeviceSelectorInfo^ DeviceSelectorChoices::BluetoothLE::get()
    {
        // Currently Bluetooth APIs don't provide a selector to get ALL devices that are both paired and non-paired.  Typically you wouldn't need this for common scenarios, but it's convenient to demonstrate the
        // various sample scenarios. 
        return ref new DeviceSelectorInfo("Bluetooth LE", DeviceClass::All, "System.Devices.Aep.ProtocolId:=\"{bb7bb05e-5972-42b5-94fc-76eaa7084d49}\"", DeviceInformationKind::AssociationEndpoint);
    }

    DeviceSelectorInfo^ DeviceSelectorChoices::BluetoothLEUnpairedOnly::get()
    {
        return ref new DeviceSelectorInfo("Bluetooth LE (unpaired)", DeviceClass::All, BluetoothLEDevice::GetDeviceSelectorFromPairingState(false));
    }

    DeviceSelectorInfo^ DeviceSelectorChoices::BluetoothLEPairedOnly::get()
    {
        return ref new DeviceSelectorInfo("Bluetooth LE (paired)", DeviceClass::All, BluetoothLEDevice::GetDeviceSelectorFromPairingState(true));
    }

    DeviceSelectorInfo^ DeviceSelectorChoices::WiFiDirect::get()
    {
        return ref new DeviceSelectorInfo("Wi-Fi Direct", DeviceClass::All, WiFiDirectDevice::GetDeviceSelector(WiFiDirectDeviceSelectorType::AssociationEndpoint));
    }

    DeviceSelectorInfo^ DeviceSelectorChoices::WiFiDirectPairedOnly::get()
    {
        return ref new DeviceSelectorInfo("Wi-Fi Direct (paired)", DeviceClass::All, WiFiDirectDevice::GetDeviceSelector());
    }

    DeviceSelectorInfo^ DeviceSelectorChoices::PointOfServicePrinter::get()
    {
        return ref new DeviceSelectorInfo("Point of Service Printer", DeviceClass::All, PosPrinter::GetDeviceSelector());
    }

    DeviceSelectorInfo^ DeviceSelectorChoices::VideoCasting::get()
    {
        return ref new DeviceSelectorInfo("Video Casting", DeviceClass::All, CastingDevice::GetDeviceSelector(CastingPlaybackTypes::Video));
    }

    DeviceSelectorInfo^ DeviceSelectorChoices::DialAllApps::get()
    {
        return ref new DeviceSelectorInfo("DIAL (All apps)", DeviceClass::All, DialDevice::GetDeviceSelector(""));
    }

    // WSD and UPnP are unique in that there are currently no general WSD or UPnP APIs to USE the devices once you've discovered them. 
    // You can pair the devices using DeviceInformation.Pairing.PairAsync etc, and you can USE them with the sockets APIs. However, since
    // there's no specific API right now, there's no *.GetDeviceSelector available.  That's why we just simply build the device selector
    // ourselves and specify the correct DeviceInformationKind (AssociationEndpoint). 
    DeviceSelectorInfo^ DeviceSelectorChoices::Wsd::get()
    {
        return ref new DeviceSelectorInfo("Web Services on Devices (WSD)", DeviceClass::All, "System.Devices.Aep.ProtocolId:=\"{782232aa-a2f9-4993-971b-aedc551346b0}\"", DeviceInformationKind::AssociationEndpoint);
    }

    DeviceSelectorInfo^ DeviceSelectorChoices::Upnp::get()
    {
        return ref new DeviceSelectorInfo("UPnP", DeviceClass::All, "System.Devices.Aep.ProtocolId:=\"{0e261de4-12f0-46e6-91ba-428607ccef64}\"", DeviceInformationKind::AssociationEndpoint);
    }


    IVectorView<DeviceSelectorInfo^>^ DeviceSelectorChoices::DevicePickerSelectors::get()
    {
        Vector<DeviceSelectorInfo^>^ selectors = ref new Vector<DeviceSelectorInfo^>(begin(CommonDeviceSelectors), end(CommonDeviceSelectors));
        selectors->Append(BluetoothPairedOnly);
        selectors->Append(BluetoothUnpairedOnly);
        selectors->Append(BluetoothLEPairedOnly);
        selectors->Append(BluetoothLEUnpairedOnly);
        selectors->Append(WiFiDirect);
        selectors->Append(PointOfServicePrinter);
        selectors->Append(VideoCasting);
        selectors->Append(DialAllApps);

        return selectors->GetView();
    }

    IVectorView<DeviceSelectorInfo^>^ DeviceSelectorChoices::FindAllAsyncSelectors::get()
    {
        Vector<DeviceSelectorInfo^>^ selectors = ref new Vector<DeviceSelectorInfo^>(begin(CommonDeviceSelectors), end(CommonDeviceSelectors));
        selectors->Append(BluetoothPairedOnly);
        selectors->Append(BluetoothLEPairedOnly);
        selectors->Append(WiFiDirectPairedOnly);

        return selectors->GetView();
    }

    IVectorView<DeviceSelectorInfo^>^ DeviceSelectorChoices::DeviceWatcherSelectors::get()
    {
        Vector<DeviceSelectorInfo^>^ selectors = ref new Vector<DeviceSelectorInfo^>(begin(CommonDeviceSelectors), end(CommonDeviceSelectors));
        selectors->Append(Bluetooth);
        selectors->Append(BluetoothLE);
        selectors->Append(WiFiDirect);
        selectors->Append(PointOfServicePrinter);
        selectors->Append(VideoCasting);
        selectors->Append(DialAllApps);
        selectors->Append(Wsd);
        selectors->Append(Upnp);

        return selectors->GetView();
    }

    IVectorView<DeviceSelectorInfo^>^ DeviceSelectorChoices::BackgroundDeviceWatcherSelectors::get()
    {
        Vector<DeviceSelectorInfo^>^ selectors = ref new Vector<DeviceSelectorInfo^>(begin(CommonDeviceSelectors), end(CommonDeviceSelectors));
        selectors->Append(BluetoothPairedOnly);
        selectors->Append(BluetoothLEPairedOnly);
        selectors->Append(WiFiDirectPairedOnly);
        selectors->Append(PointOfServicePrinter);
        selectors->Append(VideoCasting);
        selectors->Append(DialAllApps);
        selectors->Append(Wsd);
        selectors->Append(Upnp);

        return selectors->GetView();
    }

    IVectorView<DeviceSelectorInfo^>^ DeviceSelectorChoices::PairingSelectors::get()
    {
        Vector<DeviceSelectorInfo^>^ selectors = ref new Vector<DeviceSelectorInfo^>();
        selectors->Append(Bluetooth);
        selectors->Append(BluetoothLE);
        selectors->Append(WiFiDirect);
        selectors->Append(PointOfServicePrinter);
        selectors->Append(VideoCasting);
        selectors->Append(Wsd);
        selectors->Append(Upnp);

        return selectors->GetView();
    }

    // DeviceInformationKindChoices

    IVectorView<DeviceInformationKindChoice^>^ DeviceInformationKindChoices::Choices::get()
    {
        Vector<DeviceInformationKindChoice^>^ choices = ref new Vector<DeviceInformationKindChoice^>();

        Vector<DeviceInformationKind>^ kinds;
        
        kinds = ref new Vector<DeviceInformationKind>();
        kinds->Append(DeviceInformationKind::DeviceContainer);
        choices->Append(ref new DeviceInformationKindChoice("DeviceContainer", kinds->GetView()));

        kinds = ref new Vector<DeviceInformationKind>();
        kinds->Append(DeviceInformationKind::Device);
        choices->Append(ref new DeviceInformationKindChoice("Device", kinds->GetView()));

        kinds = ref new Vector<DeviceInformationKind>();
        kinds->Append(DeviceInformationKind::DeviceInterface);
        choices->Append(ref new DeviceInformationKindChoice("DeviceInterface", kinds->GetView()));

        kinds = ref new Vector<DeviceInformationKind>();
        kinds->Append(DeviceInformationKind::DeviceInterfaceClass);
        choices->Append(ref new DeviceInformationKindChoice("DeviceInterfaceClass", kinds->GetView()));

        kinds = ref new Vector<DeviceInformationKind>();
        kinds->Append(DeviceInformationKind::AssociationEndpointContainer);
        choices->Append(ref new DeviceInformationKindChoice("AssociationEndpointContainer", kinds->GetView()));

        kinds = ref new Vector<DeviceInformationKind>();
        kinds->Append(DeviceInformationKind::AssociationEndpoint);
        choices->Append(ref new DeviceInformationKindChoice("AssociationEndpoint", kinds->GetView()));

        kinds = ref new Vector<DeviceInformationKind>();
        kinds->Append(DeviceInformationKind::AssociationEndpointService);
        choices->Append(ref new DeviceInformationKindChoice("AssociationEndpointService", kinds->GetView()));

        kinds = ref new Vector<DeviceInformationKind>();
        kinds->Append(DeviceInformationKind::AssociationEndpointService);
        kinds->Append(DeviceInformationKind::DeviceInterface);
        choices->Append(ref new DeviceInformationKindChoice("AssociationEndpointService and DeviceInterface", kinds->GetView()));
        
        return choices->GetView();
    }

    IVectorView<ProtectionLevelSelectorInfo^>^ ProtectionSelectorChoices::Selectors::get()
    {
        Vector<ProtectionLevelSelectorInfo^>^ selectors = ref new Vector<ProtectionLevelSelectorInfo^>();

        selectors->Append(ref new ProtectionLevelSelectorInfo("Default", Windows::Devices::Enumeration::DevicePairingProtectionLevel::Default ));
        selectors->Append(ref new ProtectionLevelSelectorInfo("None", Windows::Devices::Enumeration::DevicePairingProtectionLevel::None ));
        selectors->Append(ref new ProtectionLevelSelectorInfo("Encryption", Windows::Devices::Enumeration::DevicePairingProtectionLevel::Encryption ));
        selectors->Append(ref new ProtectionLevelSelectorInfo("Encryption and authentication", Windows::Devices::Enumeration::DevicePairingProtectionLevel::EncryptionAndAuthentication ));

        return selectors->GetView();
    }

    // DeviceInformationDisplay

    DeviceInformationDisplay::DeviceInformationDisplay(DeviceInformation^ deviceInfoIn)
    {
        deviceInfo = deviceInfoIn;
        UpdateGlyphBitmapImage();
    }

    String^ DeviceInformationDisplay::KindString::get()
    {
        String^ str;
        switch (deviceInfo->Kind)
        {
        case DeviceInformationKind::DeviceContainer:
            str = "DeviceContainer";
            break;
        case DeviceInformationKind::Device:
            str = "Device";
            break;
        case DeviceInformationKind::DeviceInterface:
            str = "DeviceInterface";
            break;
        case DeviceInformationKind::DeviceInterfaceClass:
            str = "DeviceInterfaceClass";
            break;
        case DeviceInformationKind::AssociationEndpointContainer:
            str = "AssociationEndpointContainer";
            break;
        case DeviceInformationKind::AssociationEndpoint:
            str = "AssociationEndpoint";
            break;
        case DeviceInformationKind::AssociationEndpointService:
            str = "AssociationEndpointService";
            break;
        };

        return str;
    }

    void DeviceInformationDisplay::Update(DeviceInformationUpdate^ deviceInfoUpdateIn)
    {
        deviceInfo->Update(deviceInfoUpdateIn);

        OnPropertyChanged("Kind");
        OnPropertyChanged("Id");
        OnPropertyChanged("Name");
        OnPropertyChanged("DeviceInformation");
        OnPropertyChanged("CanPair");
        OnPropertyChanged("IsPaired");

        UpdateGlyphBitmapImage();
    }

    void DeviceInformationDisplay::UpdateGlyphBitmapImage()
    {
        BitmapImage^ glyphBitmapImage;
        glyphBitmapImage = ref new BitmapImage();

        concurrency::create_task(deviceInfo->GetGlyphThumbnailAsync()).then(
            [glyphBitmapImage](DeviceThumbnail^ result)
        {
            return glyphBitmapImage->SetSourceAsync(result);
        }).then(
            [this, glyphBitmapImage]()
        {
            GlyphBitmapImage = glyphBitmapImage;
            OnPropertyChanged("GlyphBitmapImage");
        });
    }

    void DeviceInformationDisplay::OnPropertyChanged(String^ name)
    {
        MainPage::Current->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
            [this, name]()
        {
            PropertyChanged(this, ref new PropertyChangedEventArgs(name));
        }));
    }


    // GeneralPropertyValueConverter
    
    Object^ GeneralPropertyValueConverter::Convert(
        Object^ value,
        TypeName targetType,
        Object^ parameter,
        String^ language)
    {
        Object^ property = nullptr;
        String^ strParameter = safe_cast<String^>(parameter);

        if (nullptr != strParameter && !strParameter->IsEmpty())
        {
            IMapView<String^, Object^>^ properties = safe_cast<IMapView<String^, Object^>^>(value);

            property = properties->Lookup(strParameter);
        }

        return property;
    }
}