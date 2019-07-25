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
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "SampleConfiguration.h"
#include "DeviceHelpers.h"

using namespace winrt;
using namespace winrt::SDKTemplate;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"DeviceEnumeration C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>({
    Scenario{ L"Device Picker Common Control", xaml_typename<SDKTemplate::Scenario1_DevicePicker>() },
    Scenario{ L"Enumerate and Watch Devices", xaml_typename<SDKTemplate::Scenario2_DeviceWatcher>() },
    Scenario{ L"Enumerate and Watch Devices in a Background Task", xaml_typename<SDKTemplate::Scenario3_BackgroundDeviceWatcher>() },
    Scenario{ L"Enumerate Snapshot of Devices", xaml_typename<SDKTemplate::Scenario4_Snapshot>() },
    Scenario{ L"Get Single Device", xaml_typename<SDKTemplate::Scenario5_GetSingleDevice>() },
    Scenario{ L"Custom Filter with Additional Properties", xaml_typename<SDKTemplate::Scenario6_CustomFilterAddedProps>() },
    Scenario{ L"Request Specific DeviceInformationKind", xaml_typename<SDKTemplate::Scenario7_DeviceInformationKind>() },
    Scenario{ L"Basic Device Pairing", xaml_typename<SDKTemplate::Scenario8_PairDevice>() },
    Scenario{ L"Custom Device Pairing", xaml_typename<SDKTemplate::Scenario9_CustomPairDevice>() },
});

hstring winrt::to_hstring(DevicePairingResultStatus status)
{
    switch (status)
    {
    case DevicePairingResultStatus::Paired: return L"Paired";
    case DevicePairingResultStatus::NotReadyToPair: return L"NotReadyToPair";
    case DevicePairingResultStatus::NotPaired: return L"NotPaired";
    case DevicePairingResultStatus::AlreadyPaired: return L"AlreadyPaired";
    case DevicePairingResultStatus::ConnectionRejected: return L"ConnectionRejected";
    case DevicePairingResultStatus::TooManyConnections: return L"TooManyConnections";
    case DevicePairingResultStatus::HardwareFailure: return L"HardwareFailure";
    case DevicePairingResultStatus::AuthenticationTimeout: return L"AuthenticationTimeout";
    case DevicePairingResultStatus::AuthenticationNotAllowed: return L"AuthenticationNotAllowed";
    case DevicePairingResultStatus::AuthenticationFailure: return L"AuthenticationFailure";
    case DevicePairingResultStatus::NoSupportedProfiles: return L"NoSupportedProfiles";
    case DevicePairingResultStatus::ProtectionLevelCouldNotBeMet: return L"ProtectionLevelCouldNotBeMet";
    case DevicePairingResultStatus::AccessDenied: return L"AccessDenied";
    case DevicePairingResultStatus::InvalidCeremonyData: return L"InvalidCeremonyData";
    case DevicePairingResultStatus::PairingCanceled: return L"PairingCanceled";
    case DevicePairingResultStatus::OperationAlreadyInProgress: return L"OperationAlreadyInProgress";
    case DevicePairingResultStatus::RequiredHandlerNotRegistered: return L"RequiredHandlerNotRegistered";
    case DevicePairingResultStatus::RejectedByHandler: return L"RejectedByHandler";
    case DevicePairingResultStatus::RemoteDeviceHasAssociation: return L"RemoteDeviceHasAssociation";
    case DevicePairingResultStatus::Failed: return L"Failed";
    }
    return to_hstring(static_cast<uint32_t>(status));
}

hstring winrt::to_hstring(DeviceUnpairingResultStatus status)
{
    switch (status)
    {
    case DeviceUnpairingResultStatus::Unpaired: return L"Unpaired";
    case DeviceUnpairingResultStatus::AlreadyUnpaired: return L"AlreadyUnpaired";
    case DeviceUnpairingResultStatus::OperationAlreadyInProgress: return L"OperationAlreadyInProgress";
    case DeviceUnpairingResultStatus::AccessDenied: return L"AccessDenied";
    case DeviceUnpairingResultStatus::Failed: return L"Failed";
    }
    return to_hstring(static_cast<uint32_t>(status));
}

hstring winrt::to_hstring(DeviceInformationKind kind)
{
    switch (kind)
    {
    case DeviceInformationKind::Unknown: return L"Unknown";
    case DeviceInformationKind::DeviceInterface: return L"DeviceInterface";
    case DeviceInformationKind::DeviceContainer: return L"DeviceContainer";
    case DeviceInformationKind::Device: return L"Device";
    case DeviceInformationKind::DeviceInterfaceClass: return L"DeviceInterfaceClass";
    case DeviceInformationKind::AssociationEndpoint: return L"AssociationEndpoint";
    case DeviceInformationKind::AssociationEndpointContainer: return L"AssociationEndpointContainer";
    case DeviceInformationKind::AssociationEndpointService: return L"AssociationEndpointService";
    case DeviceInformationKind::DevicePanel: return L"DevicePanel";
    }
    return to_hstring(static_cast<uint32_t>(kind));
}

#ifdef __cpp_coroutines
#error
#endif

