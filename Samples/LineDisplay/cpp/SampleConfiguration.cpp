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
using namespace Platform;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Selecting a Line Display", "SDKTemplate.Scenario1_SelectDisplay" },
    { "Displaying text", "SDKTemplate.Scenario2_DisplayText" },
    { "Using windows to control layout", "SDKTemplate.Scenario3_UsingWindows" },
    { "Updating line display attributes", "SDKTemplate.Scenario4_UpdatingLineDisplayAttributes" },
    { "Defining custom glyphs", "SDKTemplate.Scenario5_DefiningCustomGlyphs" },
    { "Modifying the display cursor", "SDKTemplate.Scenario6_ManipulatingCursorAttributes" },
    { "Scrolling content using marquee", "SDKTemplate.Scenario7_ScrollingContentUsingMarquee" }
};

Concurrency::task<ClaimedLineDisplay^> MainPage::ClaimScenarioLineDisplayAsync()
{
    ClaimedLineDisplay^ lineDisplay;
    if (LineDisplayId->IsEmpty())
    {
        NotifyUser("You must use scenario 1 to select a line display", NotifyType::ErrorMessage);
    }
    else
    {
        lineDisplay = co_await ClaimedLineDisplay::FromIdAsync(LineDisplayId);
        if (lineDisplay == nullptr)
        {
            NotifyUser("Unable to claim selected LineDisplay from id.", NotifyType::ErrorMessage);
        }
    }
    return lineDisplay;
}

void Helpers::AddItem(ComboBox^ comboBox, Object^ value, String^ name)
{
    auto item = ref new ComboBoxItem();
    item->Content = name;
    item->Tag = value;
    comboBox->Items->Append(item);
}
