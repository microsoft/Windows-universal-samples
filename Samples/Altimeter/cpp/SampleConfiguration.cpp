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

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation::Metadata;

Array<Scenario>^ MainPage::scenariosInner = ref new Array<Scenario>
{
    { "Data Events", "SDKTemplate.Scenario1_DataEvents" },
    { "Polling", "SDKTemplate.Scenario2_Polling" }
};

// This works around an issue in the Anniversary Update (1607) in which
// Altimeter::GetDefault() returns a nonfunctional altimeter if the
// system has no altimeter. This issue does not exist in other versions
// of Windows 10, but the workaround is harmless to use even on versions
// which do not have this problem. The workaround returns the default
// altimeter only after we confirm that the system has a working altimeter.

task<Altimeter^> MainPage::GetDefaultAltimeterAsync()
{
    // Run this task only once, and cache the result.
    static task<Altimeter^> altimeterTask = []()
    {
        // This workaround is needed only on the Anniversary Update (universal contract 3).
        if (!ApiInformation::IsApiContractPresent("Windows.Foundation.UniversalApiContract", 3) ||
            ApiInformation::IsApiContractPresent("Windows.Foundation.UniversalApiContract", 4))
        {
            // The current system does not require the workaround.
            return task_from_result(Altimeter::GetDefault());
        }

        String^ deviceSelector =
            // Find all interface classes for altimeter sensors
            L"System.Devices.InterfaceClassGuid:=\"{0E903829-FF8A-4A93-97DF-3DCBDE402288}\""
            // ... that are present on the system
            L" AND System.Devices.InterfaceEnabled:=System.StructuredQueryType.Boolean#True";

        DeviceWatcher^ watcher = DeviceInformation::CreateWatcher(deviceSelector);
        task_completion_event<Altimeter^> tce;
        watcher->Added += ref new Windows::Foundation::TypedEventHandler<DeviceWatcher^, DeviceInformation^>(
            [tce](DeviceWatcher^, DeviceInformation^) { tce.set(Altimeter::GetDefault()); });
        watcher->EnumerationCompleted += ref new Windows::Foundation::TypedEventHandler<DeviceWatcher^, Object^>(
            [tce](DeviceWatcher^, Object^) { tce.set(nullptr); });
        watcher->Start();

        return task<Altimeter^>(tce).then([watcher](task<Altimeter^> previousTask)
        {
            // No matter what happens, make sure we stop the watcher.
            watcher->Stop(); 
            return previousTask;
        });
    }();

    return altimeterTask;
}
