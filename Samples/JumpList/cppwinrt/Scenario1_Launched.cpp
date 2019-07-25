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
#include "Scenario1_Launched.h"
#include "Scenario1_Launched.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Launched::Scenario1_Launched()
    {
        InitializeComponent();
    }

    void Scenario1_Launched::OnNavigatedTo(NavigationEventArgs const& e)
    {
        auto argument = unbox_value_or<hstring>(e.Parameter(), hstring());
        if (!argument.empty())
        {
            ActivatedBy().Text(argument);
        }
        else
        {
            ActivatedBy().Text(L"manual navigation");
        }
    }
}
