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
#include "Scenario1.h"
#include "Scenario1.g.cpp"
#include "LoggingChannelScenario.h"

using namespace winrt;
using namespace winrt::SDKTemplate::implementation;

Scenario1::Scenario1()
{
    InitializeComponent();
}

void Scenario1::DoWin81Mode(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&)
{
    LoggingChannelScenario().LogWithWin81Constructor();
    MainPage::Current().NotifyUser(L"Complete: Windows 8 mode", NotifyType::StatusMessage);
}

void Scenario1::DoWin10Mode(IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&)
{
    LoggingChannelScenario().LogWithWin10Constructor();
    MainPage::Current().NotifyUser(L"Complete: Windows 10 mode", NotifyType::StatusMessage);
}
