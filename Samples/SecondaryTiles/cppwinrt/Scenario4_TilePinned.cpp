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
#include "Scenario4_TilePinned.h"
#include "Scenario4_TilePinned.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::StartScreen;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario4_TilePinned::Scenario4_TilePinned()
    {
        InitializeComponent();
    }

    void Scenario4_TilePinned::TileExists_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (SecondaryTile::Exists(SampleHelpers::LogoSecondaryTileId))
        {
            rootPage.NotifyUser(hstring(SampleHelpers::LogoSecondaryTileId) + L" is pinned.", NotifyType::StatusMessage);
        }
        else
        {
            rootPage.NotifyUser(hstring(SampleHelpers::LogoSecondaryTileId) + L" is not currently pinned.", NotifyType::ErrorMessage);
        }
    }
}
