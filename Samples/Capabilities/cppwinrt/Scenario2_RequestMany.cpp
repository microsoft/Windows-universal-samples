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
#include "Scenario2_RequestMany.h"
#include "Scenario2_RequestMany.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Security::Authorization::AppCapabilityAccess;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_RequestMany::Scenario2_RequestMany()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario2_RequestMany::RequestAccessButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // This will prompt for any capabilities whose current status is UserPromptRequired.
        IMapView<hstring, AppCapabilityAccessStatus> result =
            co_await AppCapability::RequestAccessForCapabilitiesAsync({ L"location", L"webcam" });
        hstring locationMessage;
        if (result.Lookup(L"location") == AppCapabilityAccessStatus::Allowed)
        {
            locationMessage = L"Location access is granted. ";
        }
        else
        {
            // We could inspect the specific status to provide better feedback,
            // but for the purpose of this sample, we merely report whether access was granted
            // and do not study the reason why.
            locationMessage = L"Location access is not granted. ";
        }
        hstring webcamMessage;
        if (result.Lookup(L"webcam") == AppCapabilityAccessStatus::Allowed)
        {
            webcamMessage = L"Webcam access is granted. ";
        }
        else
        {
            // We could inspect the specific status to provide better feedback,
            // but for the purpose of this sample, we merely report whether access was granted
            // and do not study the reason why.
            webcamMessage = L"Webcam access is not granted. ";
        }
        rootPage.NotifyUser(locationMessage + webcamMessage, NotifyType::StatusMessage);
    }
}
