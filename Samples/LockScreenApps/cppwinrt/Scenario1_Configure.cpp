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
#include "Scenario1_Configure.h"
#include "Scenario1_Configure.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::System;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Configure::Scenario1_Configure()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario1_Configure::OpenAppSettings(IInspectable const&, RoutedEventArgs const&)
    {
        co_await Launcher::LaunchUriAsync(Uri(L"ms-settings:appsfeatures-app"));
    }

    fire_and_forget Scenario1_Configure::OpenLockscreenSettings(IInspectable const&, RoutedEventArgs const&)
    {
        co_await Launcher::LaunchUriAsync(Uri(L"ms-settings:lockscreen"));
    }
}
