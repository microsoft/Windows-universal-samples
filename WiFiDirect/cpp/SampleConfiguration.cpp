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
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace WiFiDirectApi;
using namespace Windows::UI::Core;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Advertiser", "WiFiDirectApi.Scenario1_Advertiser" },
    { "Connector", "WiFiDirectApi.Scenario2_Connector" }
};

void MainPage::NotifyUserFromBackground(Platform::String^ strMessage, WiFiDirectApi::NotifyType type)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, strMessage, type]()
    {
        NotifyUser(strMessage, type);
    }));
}
