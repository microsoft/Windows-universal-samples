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
#include "Scenario1_ToggleRadios.xaml.h"
#include "RadioModel.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Radios;
using namespace concurrency;

Scenario1_ToggleRadios::Scenario1_ToggleRadios() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void SDKTemplate::Scenario1_ToggleRadios::OnNavigatedTo(NavigationEventArgs^ e)
{
    // RequestAccessAsync must be called at least once from the UI thread
    auto requestAccessOperation = Radio::RequestAccessAsync();
    create_task(requestAccessOperation).then([this](task<RadioAccessStatus> asyncInfo)
    {
        RadioAccessStatus accessLevel = asyncInfo.get();
        if (accessLevel != RadioAccessStatus::Allowed)
        {
            rootPage->NotifyUser("App is not allowed to control radios.", NotifyType::ErrorMessage);
        }
        else
        {
            InitializeRadios();
        }
    });
}

void SDKTemplate::Scenario1_ToggleRadios::InitializeRadios()
{
    // An alternative to Radio::GetRadiosAsync is to use the Windows::Devices::Enumeration pattern,
    // passing Radio::GetDeviceSelector as the AQS string
    auto getRadiosOperation = Radio::GetRadiosAsync();
    create_task(getRadiosOperation).then([this](task<IVectorView<Radio^>^> asyncInfo)
    {
        auto radios = asyncInfo.get();
        for (auto radio : radios)
        {
            RadioSwitchList->Items->Append(ref new RadioModel(radio, this));
        }
    });
}

