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
#include "Scenario1_Launched.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_Launched::Scenario1_Launched()
{
    InitializeComponent();
}

void Scenario1_Launched::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    if (MainPage::Current->LaunchParam != nullptr && !MainPage::Current->LaunchParam->IsEmpty())
    {
        ActivatedBy->Text = MainPage::Current->LaunchParam;
    }
    else
    {
        ActivatedBy->Text = "manual navigation.";
    }
	MainPage::Current->LaunchParam = nullptr;
}