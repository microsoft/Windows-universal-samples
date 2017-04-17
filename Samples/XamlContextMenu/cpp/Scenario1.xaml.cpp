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

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;

Scenario1::Scenario1()
{
    InitializeComponent();
}

void Scenario1::OpenMenuItem_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Action: Open", NotifyType::StatusMessage);
}

void Scenario1::PrintMenuItem_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Action: Print", NotifyType::StatusMessage);
}

void Scenario1::DeleteMenuItem_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Action: Delete", NotifyType::StatusMessage);
}
