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
using namespace Windows::Graphics::Imaging;
using namespace SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"Basic face tracking C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Track Faces in Webcam", xaml_typename<SDKTemplate::Scenario1_TrackInWebcam>() },
});

hstring winrt::to_hstring(BitmapPixelFormat format)
{
    switch (format)
    {
    case BitmapPixelFormat::Unknown: return L"Unknown";
    case BitmapPixelFormat::Rgba16: return L"Rgba16";
    case BitmapPixelFormat::Rgba8: return L"Rgba8";
    case BitmapPixelFormat::Gray16: return L"Gray16";
    case BitmapPixelFormat::Gray8: return L"Gray8";
    case BitmapPixelFormat::Bgra8: return L"Bgra8";
    case BitmapPixelFormat::Nv12: return L"Nv12";
    case BitmapPixelFormat::P010: return L"P010";
    case BitmapPixelFormat::Yuy2: return L"Yuy2";
    }
    return to_hstring(static_cast<int32_t>(format));
}