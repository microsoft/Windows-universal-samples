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

#pragma once
#include "pch.h"

namespace winrt::SDKTemplate
{
    bool TryParseUInt(const wchar_t* str, uint32_t& result);
    wchar_t* TryParseUIntPrefix(const wchar_t* str, uint32_t& result);
    bool TryParseDouble(const wchar_t* str, double& result);

    hstring FormatDateTime(Windows::Foundation::DateTime value, hstring const& format);

    Windows::ApplicationModel::Background::IBackgroundTaskRegistration LookupBackgroundTask(hstring const& name);
    fire_and_forget CheckBackgroundAndRequestLocationAccess();

    void FillItemsFromSavedJson(Windows::UI::Xaml::Controls::ItemsControl const& itemsControl, hstring const& key);
    void ReportSavedStatus();

    hstring to_message();
}

namespace winrt
{
    hstring to_hstring(Windows::Foundation::IReference<double> const& value);
    hstring to_hstring(Windows::Foundation::TimeSpan value);
    hstring to_hstring(Windows::Devices::Geolocation::PositionSource value);
    hstring to_hstring(Windows::Devices::Geolocation::VisitStateChange value);
    hstring to_hstring(Windows::Devices::Geolocation::Geofencing::GeofenceState value);
    hstring to_hstring(Windows::Devices::Geolocation::Geofencing::GeofenceMonitorStatus value);
    hstring to_hstring(Windows::Devices::Geolocation::Geofencing::GeofenceRemovalReason value);
}
