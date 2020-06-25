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
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "SampleConfiguration.h"
#include <ctime>

using namespace winrt;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;
using namespace SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"SecondaryTiles C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Pin Tile",                            xaml_typename<SDKTemplate::Scenario1_PinTile>() },
    Scenario{ L"Unpin Tile",                          xaml_typename<SDKTemplate::Scenario2_UnpinTile>() },
    Scenario{ L"Enumerate Tiles",                     xaml_typename<SDKTemplate::Scenario3_EnumerateTiles>() },
    Scenario{ L"Is Tile Pinned?",                     xaml_typename<SDKTemplate::Scenario4_TilePinned>() },
    Scenario{ L"Show Activation Arguments",           xaml_typename<SDKTemplate::Scenario5_LaunchedFromSecondaryTile>() },
    Scenario{ L"Secondary Tile Notifications",        xaml_typename<SDKTemplate::Scenario6_SecondaryTileNotification>() },
    Scenario{ L"Pin/Unpin Through Appbar",            xaml_typename<SDKTemplate::Scenario7_PinFromAppbar>() },
    Scenario{ L"Update Secondary Tile Default Logo",  xaml_typename<SDKTemplate::Scenario8_UpdateAsync>() },
});

hstring SampleHelpers::CurrentTimeAsString()
{
    std::time_t now = clock::to_time_t(clock::now());
    char buffer[26];
    ctime_s(buffer, ARRAYSIZE(buffer), &now);
    return to_hstring(buffer);
}

Rect SampleHelpers::GetElementRect(IInspectable const& e)
{
    auto element = e.as<FrameworkElement>();
    GeneralTransform transform = element.TransformToVisual(nullptr);
    return transform.TransformBounds(Rect{ 0, 0, static_cast<float>(element.ActualWidth()), static_cast<float>(element.ActualHeight()) });
}

void winrt::SDKTemplate::App_LaunchCompleted(LaunchActivatedEventArgs const& e)
{
    hstring arguments = e.Arguments();
    if (!arguments.empty())
    {
        implementation::MainPage::Current().Navigate(xaml_typename<Scenario5_LaunchedFromSecondaryTile>(), box_value(arguments));
    }
}
