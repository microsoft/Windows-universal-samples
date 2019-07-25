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
#include "Scenario5_GetSingleDevice.h"
#include "SampleConfiguration.h"
#include "DeviceHelpers.h"
#include "Scenario5_GetSingleDevice.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario5_GetSingleDevice::Scenario5_GetSingleDevice()
    {
        InitializeComponent();
    }

    void Scenario5_GetSingleDevice::OnNavigatedTo(NavigationEventArgs const&)
    {
        resultsListView().ItemsSource(resultCollection);
    }

    fire_and_forget Scenario5_GetSingleDevice::Page_Loaded(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Pre-populate the text box with an interface id.
         DeviceInformationCollection deviceInfoCollection = co_await DeviceInformation::FindAllAsync(DeviceClass::AudioRender);
        if (deviceInfoCollection.Size() > 0)
        {
            // When you want to "save" a DeviceInformation to get it back again later,
            // use both the DeviceInformation.Kind and the DeviceInformation.Id.
            DeviceInformation info = deviceInfoCollection.GetAt(0);
            interfaceIdTextBox().Text(info.Id());
            deviceInformationKind = info.Kind();
            InformationKindTextBox().Text(to_hstring(deviceInformationKind));
            getButton().IsEnabled(true);
        }
    }

    fire_and_forget Scenario5_GetSingleDevice::GetButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        resultCollection.Clear();

        hstring interfaceId = interfaceIdTextBox().Text();
        getButton().IsEnabled(false);

        rootPage.NotifyUser(L"CreateFromIdAsync operation started...", NotifyType::StatusMessage);

        try
        {
            // When you want to "save" a DeviceInformation to get it back again later,
            // use both the DeviceInformation.Kind and the DeviceInformation.Id.
            DeviceInformation deviceInfo = co_await DeviceInformation::CreateFromIdAsync(interfaceId, nullptr, deviceInformationKind);

            rootPage.NotifyUser(L"CreateFromIdAsync operation completed.", NotifyType::StatusMessage);

            resultCollection.Append(make<DeviceInformationDisplay>(deviceInfo));
        }
        catch (hresult_error const& e)
        {
            if (e.code() == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            {
                rootPage.NotifyUser(L"Invalid Interface Id", NotifyType::ErrorMessage);
            }
            else
            {
                throw;
            }
        }

        getButton().IsEnabled(true);
        getButton().Focus(FocusState::Keyboard);
    }
}
