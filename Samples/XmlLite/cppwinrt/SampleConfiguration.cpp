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
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"XmlLite C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"XmlLite Reader", xaml_typename<SDKTemplate::Scenario1_XmlReader>() },
    Scenario{ L"Write XML with XmlWriter", xaml_typename<SDKTemplate::Scenario2_XmlWriter>() },
    Scenario{ L"Write XML with XmlWriterLite", xaml_typename<SDKTemplate::Scenario3_XmlWriterLite>() },
});
