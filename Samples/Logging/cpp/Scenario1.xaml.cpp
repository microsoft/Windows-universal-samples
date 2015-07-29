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
#include "Scenario1.xaml.h"
#include "LoggingChannelScenario.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::UI::Xaml;

Scenario1::Scenario1()
    : rootPage(MainPage::Current)
{
    InitializeComponent();
}

/// <summary>
/// This method demonstrates logging with a LoggingChannel constructed
/// in Windows 8.1 compatibility mode.
/// </summary>
void Scenario1::DoWin81Mode(
    Object^ sender,
    RoutedEventArgs^ e)
{
    (ref new LoggingChannelScenario())->LogWithWin81Constructor();
    rootPage->NotifyUser("Complete: Windows 8.1 mode", NotifyType::StatusMessage);
}

/// <summary>
/// This method demonstrates LoggingChannel in Windows 10 mode.
/// </summary>
void Scenario1::DoWin10Mode(
    Object^ sender,
    RoutedEventArgs^ e)
{
    (ref new LoggingChannelScenario())->LogWithWin10Constructor();
    rootPage->NotifyUser("Complete: Windows 10 mode", NotifyType::StatusMessage);
}
