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
#include "Scenario1_Basic.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::UI::Xaml;

Scenario1_Basic::Scenario1_Basic()
{
    InitializeComponent();
}

void Scenario1_Basic::ClickToPlaySource_ImageOpened(Object^ sender, RoutedEventArgs^ e)
{
    // Once ImageOpened is raised, we can query whether an image is animated.
    if (ClickToPlaySource->IsAnimatedBitmap)
    {
        PlaybackButtons->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
}
