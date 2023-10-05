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

using namespace winrt;
using namespace winrt::SDKTemplate;
using namespace winrt::Windows::Devices::Lights;
using namespace winrt::Windows::Foundation::Collections;

hstring winrt::to_hstring(LampArrayKind lampArrayKind)
{
    switch (lampArrayKind)
    {
    case LampArrayKind::Keyboard:
        return L"Keyboard";
    case LampArrayKind::Mouse:
        return L"Mouse";
    case LampArrayKind::GameController:
        return L"Game Controller";
    case LampArrayKind::Peripheral:
        return L"Peripheral";
    case LampArrayKind::Scene:
        return L"Scene";
    case LampArrayKind::Notification:
        return L"Notification";
    case LampArrayKind::Chassis:
        return L"Chassis";
    case LampArrayKind::Wearable:
        return L"Wearable";
    case LampArrayKind::Furniture:
        return L"Furniture";
    case LampArrayKind::Art:
        return L"Art";
    case LampArrayKind::Undefined:
    default:
        return L"Unknown LampArrayKind";
    }
}

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"LampArray C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"LampArray Basics", xaml_typename<SDKTemplate::Scenario1_Basics>() },
    Scenario{ L"LampArray Effects", xaml_typename<SDKTemplate::Scenario2_Effects>() },
});
