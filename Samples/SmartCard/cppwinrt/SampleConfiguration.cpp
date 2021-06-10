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
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::SDKTemplate;
using namespace winrt::Windows::Devices::SmartCards;
using namespace winrt::Windows::Foundation;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"Smart Card C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{L"Create and Provision a TPM Virtual Smart Card", xaml_typename<Scenario1_ProvisionTPM>()},
    Scenario{L"Change Smart Card PIN", xaml_typename<Scenario2_ChangePIN>()},
    Scenario{L"Reset Smart Card PIN", xaml_typename<Scenario3_ResetPIN>()},
    Scenario{L"Change Smart Card Admin Key", xaml_typename<Scenario4_ChangeAdminKey>()},
    Scenario{L"Verify Response", xaml_typename<Scenario5_VerifyResponse>()},
    Scenario{L"Delete TPM Virtual Smart Card", xaml_typename<Scenario6_DeleteTPM>()},
    Scenario{L"List All Smart Cards", xaml_typename<Scenario7_ListAllCards>()},
    Scenario{L"Transmit APDU to Smart Card", xaml_typename<Scenario8_TransmitAPDU>()}
});
