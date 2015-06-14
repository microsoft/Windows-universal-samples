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
using namespace Windows::Devices::Bluetooth::Rfcomm;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Devices::WiFiDirect;

namespace DeviceEnumerationCpp
{
    // DeviceSelectorChoices

    IVectorView<DeviceSelectorInfo^>^ DeviceSelectorChoices::Selectors::get()
    {
        Vector<DeviceSelectorInfo^>^ selectors = ref new Vector<DeviceSelectorInfo^>();

        // Pre-canned device class selectors
        selectors->Append(ref new DeviceSelectorInfo("All Device Interfaces", DeviceClass::All, nullptr));
        selectors->Append(ref new DeviceSelectorInfo("Audio Capture Devices", DeviceClass::AudioCapture, nullptr));
        selectors->Append(ref new DeviceSelectorInfo("Audio Render Devices", DeviceClass::AudioRender, nullptr));
        selectors->Append(ref new DeviceSelectorInfo("Image Scanner Devices", DeviceClass::ImageScanner, nullptr));
        selectors->Append(ref new DeviceSelectorInfo("Location Devices", DeviceClass::Location, nullptr));
        selectors->Append(ref new DeviceSelectorInfo("Portable Storage Devices", DeviceClass::PortableStorageDevice, nullptr));
        selectors->Append(ref new DeviceSelectorInfo("Video Capture Devices", DeviceClass::VideoCapture, nullptr));
        
        selectors->Append(ref new DeviceSelectorInfo("Proximity Devices", DeviceClass::All, ProximityDevice::GetDeviceSelector()));
        selectors->Append(ref new DeviceSelectorInfo("Human Interface Devices", DeviceClass::All, HidDevice::GetDeviceSelector(0, 0)));
        selectors->Append(ref new DeviceSelectorInfo("Bluetooth Rfcomm Chat Services", DeviceClass::All, RfcommDeviceService::GetDeviceSelector(RfcommServiceId::SerialPort)));
        selectors->Append(ref new DeviceSelectorInfo("Bluetooth Gatt Heart Rate Devices", DeviceClass::All, GattDeviceService::GetDeviceSelectorFromUuid(GattServiceUuids::HeartRate)));
        selectors->Append(ref new DeviceSelectorInfo("WiFi Direct Devices", DeviceClass::All, WiFiDirectDevice::GetDeviceSelector()));
        
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

    // ProtocolSelectorChoices

    IVectorView<ProtocolSelectorInfo^>^ ProtocolSelectorChoices::Selectors::get()
    {
        Vector<ProtocolSelectorInfo^>^ selectors = ref new Vector<ProtocolSelectorInfo^>();

        // Pre-canned device class selectors
        selectors->Append(ref new ProtocolSelectorInfo("Wi-Fi Direct", "System.Devices.Aep.ProtocolId:=\"{0407d24e-53de-4c9a-9ba1-9ced54641188}\""));
        selectors->Append(ref new ProtocolSelectorInfo("WSD", "System.Devices.Aep.ProtocolId:=\"{782232aa-a2f9-4993-971b-aedc551346b0}\""));
        selectors->Append(ref new ProtocolSelectorInfo("UPnP", "System.Devices.Aep.ProtocolId:=\"{0e261de4-12f0-46e6-91ba-428607ccef64}\""));
        selectors->Append(ref new ProtocolSelectorInfo("Point of Service", "System.Devices.Aep.ProtocolId:=\"{D4BF61B3-442E-4ADA-882D-FA7B70C832D9}\""));

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