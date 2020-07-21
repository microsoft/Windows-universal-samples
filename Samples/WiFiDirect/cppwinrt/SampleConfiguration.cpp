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

using namespace winrt;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::WiFiDirect;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml::Controls;
using namespace SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"Wi-Fi Direct";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Advertiser", xaml_typename<SDKTemplate::Scenario1_Advertiser>() },
    Scenario{ L"Connector", xaml_typename<SDKTemplate::Scenario2_Connector>() },
});

static IAsyncAction ShowPinToUserAsync(CoreDispatcher dispatcher, hstring strPin)
{
    co_await resume_foreground(dispatcher);

    MessageDialog messageDialog(L"Enter this PIN on the remote device: " + strPin);

    // Add commands
    messageDialog.Commands().Append(UICommand(L"OK", nullptr, 0));

    // Set the command that will be invoked by default 
    messageDialog.DefaultCommandIndex(0);

    // Set the command that will be invoked if the user cancels
    messageDialog.CancelCommandIndex(0);

    // Show the Pin 
    co_await messageDialog.ShowAsync();
}

static IAsyncOperation<hstring> GetPinFromUserAsync(CoreDispatcher dispatcher)
{
    co_await resume_foreground(dispatcher);

    TextBox pinBox;
    ContentDialog dialog;
    dialog.Title(box_value(L"Enter Pin"));
    dialog.PrimaryButtonText(L"OK");
    dialog.Content(pinBox);

    co_await dialog.ShowAsync();
    co_return pinBox.Text();
}

fire_and_forget winrt::SDKTemplate::HandlePairing(CoreDispatcher dispatcher, DevicePairingRequestedEventArgs e)
{
    auto deferral = e.GetDeferral();
    switch (e.PairingKind())
    {
        case DevicePairingKinds::DisplayPin:
            co_await ShowPinToUserAsync(dispatcher, e.Pin());
            e.Accept();
            break;

        case DevicePairingKinds::ConfirmOnly:
            e.Accept();
            break;

        case DevicePairingKinds::ProvidePin:
        {
            hstring pin = co_await GetPinFromUserAsync(dispatcher);
            if (!pin.empty())
            {
                e.Accept(pin);
            }
        }
            break;
    }
}

hstring winrt::to_hstring(DevicePairingResultStatus status)
{
    switch (status)
    {
    case DevicePairingResultStatus::Paired:
        return L"Paired";
    case DevicePairingResultStatus::NotReadyToPair:
        return L"NotReadyToPair";
    case DevicePairingResultStatus::NotPaired:
        return L"NotPaired";
    case DevicePairingResultStatus::AlreadyPaired:
        return L"AlreadyPaired";
    case DevicePairingResultStatus::ConnectionRejected:
        return L"ConnectionRejected";
    case DevicePairingResultStatus::TooManyConnections:
        return L"TooManyConnections";
    case DevicePairingResultStatus::HardwareFailure:
        return L"HardwareFailure";
    case DevicePairingResultStatus::AuthenticationTimeout:
        return L"AuthenticationTimeout";
    case DevicePairingResultStatus::AuthenticationNotAllowed:
        return L"AuthenticationNotAllowed";
    case DevicePairingResultStatus::AuthenticationFailure:
        return L"AuthenticationFailure";
    case DevicePairingResultStatus::NoSupportedProfiles:
        return L"NoSupportedProfiles";
    case DevicePairingResultStatus::ProtectionLevelCouldNotBeMet:
        return L"ProtectionLevelCouldNotBeMet";
    case DevicePairingResultStatus::AccessDenied:
        return L"AccessDenied";
    case DevicePairingResultStatus::InvalidCeremonyData:
        return L"InvalidCeremonyData";
    case DevicePairingResultStatus::PairingCanceled:
        return L"PairingCanceled";
    case DevicePairingResultStatus::OperationAlreadyInProgress:
        return L"OperationAlreadyInProgress";
    case DevicePairingResultStatus::RequiredHandlerNotRegistered:
        return L"RequiredHandlerNotRegistered";
    case DevicePairingResultStatus::RejectedByHandler:
        return L"RejectedByHandler";
    case DevicePairingResultStatus::RemoteDeviceHasAssociation:
        return L"RemoteDeviceHasAssociation";
    case DevicePairingResultStatus::Failed:
        return L"Failed";
    }
    return to_hstring(static_cast<uint32_t>(status));
}

hstring winrt::to_hstring(DeviceUnpairingResultStatus status)
{
    switch (status)
    {
    case DeviceUnpairingResultStatus::Unpaired:
        return L"Unpaired";
    case DeviceUnpairingResultStatus::AlreadyUnpaired:
        return L"AlreadyUnpaired";
    case DeviceUnpairingResultStatus::OperationAlreadyInProgress:
        return L"OperationAlreadyInProgress";
    case DeviceUnpairingResultStatus::AccessDenied:
        return L"AccessDenied";
    case DeviceUnpairingResultStatus::Failed:
        return L"Failed";
    }
    return to_hstring(static_cast<uint32_t>(status));
}

hstring winrt::to_hstring(WiFiDirectAdvertisementPublisherStatus status)
{
    switch (status)
    {
    case WiFiDirectAdvertisementPublisherStatus::Created:
        return L"Created";
    case WiFiDirectAdvertisementPublisherStatus::Started:
        return L"Started";
    case WiFiDirectAdvertisementPublisherStatus::Stopped:
        return L"Stopped";
    case WiFiDirectAdvertisementPublisherStatus::Aborted:
        return L"Aborted";
    }
    return to_hstring(static_cast<uint32_t>(status));
}

hstring winrt::to_hstring(WiFiDirectConfigurationMethod method)
{
    switch (method)
    {
    case WiFiDirectConfigurationMethod::ProvidePin:
        return L"ProvidePin";
    case WiFiDirectConfigurationMethod::DisplayPin:
        return L"DisplayPin";
    case WiFiDirectConfigurationMethod::PushButton:
        return L"PushButton";
    }
    return to_hstring(static_cast<uint32_t>(method));
}

hstring winrt::to_hstring(WiFiDirectConnectionStatus status)
{
    switch (status)
    {
    case WiFiDirectConnectionStatus::Disconnected:
        return L"Disconnected";
    case WiFiDirectConnectionStatus::Connected:
        return L"Connected";
    }
    return to_hstring(static_cast<uint32_t>(status));
}

hstring winrt::to_hstring(WiFiDirectError error)
{
    switch (error)
    {
    case WiFiDirectError::Success:
        return L"Success";
    case WiFiDirectError::RadioNotAvailable:
        return L"RadioNotAvailable";
    case WiFiDirectError::ResourceInUse:
        return L"ResourceInUse";
    }
    return to_hstring(static_cast<uint32_t>(error));
}