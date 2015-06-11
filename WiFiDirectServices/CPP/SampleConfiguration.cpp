// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;
using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Create Advertiser", "SDKTemplate.WiFiDirectServices.Scenario1" },
    { "Advertiser Accept", "SDKTemplate.WiFiDirectServices.Scenario2" },
    { "Discover/Connect Services", "SDKTemplate.WiFiDirectServices.Scenario3" },
    { "Select Session (Seeker)", "SDKTemplate.WiFiDirectServices.Scenario4" },
    { "Send Data", "SDKTemplate.WiFiDirectServices.Scenario5" }
};

void MainPage::GoToScenario(unsigned int index)
{
    if (index >= scenariosInner->Length)
    {
        throw ref new OutOfBoundsException;
    }

    ScenarioControl->SelectedIndex = index;
}