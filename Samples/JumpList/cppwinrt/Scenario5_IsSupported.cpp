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
#include "Scenario5_IsSupported.h"
#include "Scenario5_IsSupported.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::StartScreen;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario5_IsSupported::Scenario5_IsSupported()
    {
        InitializeComponent();
    }

    void Scenario5_IsSupported::OnNavigatedTo(NavigationEventArgs const&)
    {
        Supported().Text(JumpList::IsSupported() ? L"supports" : L"does not support");
    }
}
