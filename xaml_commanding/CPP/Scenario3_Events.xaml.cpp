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
#include "Scenario3_Events.xaml.h"
#include "MainPage.xaml.h"

using namespace Commanding;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario3_Events::Scenario3_Events()
{
    InitializeComponent();

	MainPage::Current->NotifyUser("CommandBar: Background Opacity at 50%", NotifyType::StatusMessage);
}


void Scenario3_Events::CommandBar_Opening(Platform::Object^ sender, Platform::Object^ e)
{
	translucentBackgroundBrush->Opacity = 1.0;
	MainPage::Current->NotifyUser("CommandBar: OPENING.  Setting Background Opacity to 100%", NotifyType::ErrorMessage);
}


void Scenario3_Events::CommandBar_Closing(Platform::Object^ sender, Platform::Object^ e)
{
	MainPage::Current->NotifyUser("CommandBar: CLOSING", NotifyType::ErrorMessage);
}


void Scenario3_Events::CommandBar_Opened(Platform::Object^ sender, Platform::Object^ e)
{
	MainPage::Current->NotifyUser("CommandBar: OPENED", NotifyType::StatusMessage);
}


void Scenario3_Events::CommandBar_Closed(Platform::Object^ sender, Platform::Object^ e)
{
	translucentBackgroundBrush->Opacity = 0.5;
	MainPage::Current->NotifyUser("CommandBar: CLOSED.  Setting Background Opacity to 50%", NotifyType::StatusMessage);
}
