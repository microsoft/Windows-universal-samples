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
#include "RadioModel.h"
#include "Scenario1_Toggle.h"
#include "Scenario1_Toggle.g.cpp"

using namespace winrt;
using namespace Windows::Devices::Radios;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Toggle::Scenario1_Toggle()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario1_Toggle::OnNavigatedTo(NavigationEventArgs const&)
    {
        // RequestAccessAsync must be called at least one from the UI thread.
        RadioAccessStatus accessLevel = co_await Radio::RequestAccessAsync();
        if (accessLevel != RadioAccessStatus::Allowed)
        {
            rootPage.NotifyUser(L"App is not allowed to control radios.", NotifyType::ErrorMessage);
        }
        else
        {
            // An alternative to Radio.GetRadiosAsync is to use the Windows.Devices.Enumeration pattern,
            // passing Radio.GetDeviceSelector as the AQS string.
            for (Radio radio : co_await Radio::GetRadiosAsync())
            {
                RadioSwitchList().Items().Append(make<RadioModel>(radio, Dispatcher()));
            }
        }
    }
}
