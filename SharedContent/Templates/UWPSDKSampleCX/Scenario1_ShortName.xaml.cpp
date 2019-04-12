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
#include "Scenario1_ShortName.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;

Scenario1_ShortName::Scenario1_ShortName()
{
    InitializeComponent();
}

void SDKTemplate::Scenario1_ShortName::StatusMessage_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Hello", NotifyType::StatusMessage);
}

void SDKTemplate::Scenario1_ShortName::ErrorMessage_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Oh dear.\nSomething went wrong.", NotifyType::ErrorMessage);
}

void SDKTemplate::Scenario1_ShortName::ClearMessage_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->NotifyUser("", NotifyType::StatusMessage);
}
