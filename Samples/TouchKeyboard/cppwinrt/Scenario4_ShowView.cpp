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
#include "Scenario4_ShowView.h"
#include "Scenario4_ShowView.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::ViewManagement::Core;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario4_ShowView::Scenario4_ShowView()
    {
        InitializeComponent();
    }

    void Scenario4_ShowView::TextBox_GotFocus(IInspectable const&, RoutedEventArgs const&)
    {
        CoreInputView::GetForCurrentView().TryShow(CoreInputViewKind::Emoji);
    }
}
