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
#include "Scenario5_LaunchedFromSecondaryTile.h"
#include "Scenario5_LaunchedFromSecondaryTile.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario5_LaunchedFromSecondaryTile::Scenario5_LaunchedFromSecondaryTile()
    {
        InitializeComponent();
    }

    void Scenario5_LaunchedFromSecondaryTile::OnNavigatedTo(NavigationEventArgs const& e)
    {
        auto launchParam = unbox_value_or<hstring>(e.Parameter(), {});
        if (!launchParam.empty())
            {
                rootPage.NotifyUser(L"Application was activated from a Secondary Tile with the following Activation Arguments: " + launchParam, NotifyType::StatusMessage);
            }

    }
}
