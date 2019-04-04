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

using namespace winrt;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"FolderEnumeration C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Enumerate files and folders in the Pictures library",        xaml_typename<SDKTemplate::Scenario1>() },
    Scenario{ L"Enumerate files in the Pictures library, by groups",         xaml_typename<SDKTemplate::Scenario2>() },
    Scenario{ L"Enumerate files in the Pictures library with prefetch APIs", xaml_typename<SDKTemplate::Scenario3>() },
    Scenario{ L"Enumerate files in a folder and display availability",       xaml_typename<SDKTemplate::Scenario4>() },
});


TextBlock Helpers::CreateHeaderTextBlock(param::hstring const& contents)
{
    TextBlock textBlock;
    textBlock.Text(contents);
    textBlock.Style(Application::Current().Resources().Lookup(box_value(L"SampleHeaderTextStyle")).as<::Style>());
    textBlock.TextWrapping(TextWrapping::Wrap);
    return textBlock;
}


TextBlock Helpers::CreateLineItemTextBlock(param::hstring const& contents)
{
    TextBlock textBlock;
    textBlock.Text(contents);
    textBlock.Style(Application::Current().Resources().Lookup(box_value(L"BasicTextStyle")).as<::Style>());
    textBlock.TextWrapping(TextWrapping::Wrap);
    Thickness margin = textBlock.Margin();
    margin.Left = 20;
    textBlock.Margin(margin);
    return textBlock;

}
