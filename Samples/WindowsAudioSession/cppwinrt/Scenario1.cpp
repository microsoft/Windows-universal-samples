//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "Scenario1.h"
#include "Scenario1.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Media::Devices;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;

namespace winrt::SDKTemplate::implementation
{
    Scenario1::Scenario1()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario1::Enumerate_Click(IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        DevicesList().Items().Clear();

        // Get the string identifier of the audio renderer
        hstring AudioSelector = MediaDevice::GetAudioRenderSelector();

        // Custom properties defined in mmdeviceapi.h in the format "{GUID} PID"
        constexpr wchar_t PKEY_AudioEndpoint_Supports_EventDriven_Mode[] = L"{1da5d803-d492-4edd-8c23-e0c0ffee7f0e} 7";

        // Add custom properties to the query
        DeviceInformationCollection deviceInfoCollection = co_await DeviceInformation::FindAllAsync(AudioSelector, { PKEY_AudioEndpoint_Supports_EventDriven_Mode });
        try
        {
            // Enumerate through the devices and the custom properties
            for (DeviceInformation&& deviceInfo : deviceInfoCollection)
            {
                hstring deviceInfoString = deviceInfo.Name();

                // Pull out the custom property
                std::optional<uint32_t> supportsEventDriven = deviceInfo.Properties().TryLookup(PKEY_AudioEndpoint_Supports_EventDriven_Mode).try_as<uint32_t>();
                if (supportsEventDriven)
                {
                    deviceInfoString = deviceInfoString + L" --> EventDriven(" + to_hstring(supportsEventDriven.value()) + L")";
                }

                DevicesList().Items().Append(box_value(deviceInfoString));
            }
            rootPage.NotifyUser(L"Enumerated " + to_hstring(deviceInfoCollection.Size()) + L" device(s).", NotifyType::StatusMessage);
        }
        catch (...)
        {
            rootPage.NotifyUser(to_message(), NotifyType::ErrorMessage);
        }
    }
}