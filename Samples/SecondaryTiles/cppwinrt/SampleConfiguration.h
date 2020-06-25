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

#pragma once 
#include "pch.h"

namespace winrt::SDKTemplate
{
    void App_LaunchCompleted(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const&);

    struct SampleHelpers
    {
        static constexpr wchar_t LogoSecondaryTileId[] = L"SecondaryTile.Logo";
        static constexpr wchar_t DynamicTileId[] = L"SecondaryTile.LiveTile";
        static constexpr wchar_t AppBarTileId[] = L"SecondaryTile.AppBar";

        static hstring CurrentTimeAsString();
        static Windows::Foundation::Rect GetElementRect(Windows::Foundation::IInspectable const& e);
    };
}
