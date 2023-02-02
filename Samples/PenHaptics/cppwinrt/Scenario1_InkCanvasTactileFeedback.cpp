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
#include "Scenario1_InkCanvasTactileFeedback.h"
#include "Scenario1_InkCanvasTactileFeedback.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_InkCanvasTactileFeedback::Scenario1_InkCanvasTactileFeedback()
    {
        InitializeComponent();
    }

    void Scenario1_InkCanvasTactileFeedback::OnSizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        outputGrid().Width(RootGrid().ActualWidth());
        outputGrid().Height(RootGrid().ActualHeight() / 2);
    }
}
