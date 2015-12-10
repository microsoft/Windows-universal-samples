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
#include "Scenario3_CustomItems.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::StartScreen;

Scenario3_CustomItems::Scenario3_CustomItems()
{
    InitializeComponent();
}

void Scenario3_CustomItems::DisplayNameLocalizedResource_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    DisplayName->IsEnabled = !DisplayNameLocalizedResource->IsChecked;
    DisplayName->Text = "ms-resource:///Resources/CustomJumpListItemDisplayName";
}

void Scenario3_CustomItems::DescriptionLocalizedResource_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    Description->IsEnabled = !DescriptionLocalizedResource->IsChecked;
    Description->Text = "ms-resource:///Resources/CustomJumpListItemDescription";
}

void Scenario3_CustomItems::GroupNameLocalizedResource_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    GroupName->IsEnabled = !GroupNameLocalizedResource->IsChecked;
    GroupName->Text = "ms-resource:///Resources/CustomJumpListGroupName";
}

void Scenario3_CustomItems::AddToJumpList_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
	create_task(JumpList::LoadCurrentAsync()).then([this](JumpList^ jumpList) -> IAsyncAction^
	{
		jumpList->SystemGroupKind = JumpListSystemGroupKind::Recent;

		auto item = JumpListItem::CreateWithArguments(Arguments->Text, DisplayName->Text);
		item->Description = Description->Text;
		item->GroupName = GroupName->Text;
		item->Logo = ref new Uri("ms-appx:///Assets/smalltile-sdk.png");

		jumpList->Items->Append(item);

		return jumpList->SaveAsync();
	}, task_continuation_context::use_current());
}