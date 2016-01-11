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
#include "Scenario4_ChangeSystemGroup.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::StartScreen;
using namespace Windows::Storage;
using namespace Windows::Storage::AccessCache;



Scenario4_ChangeSystemGroup::Scenario4_ChangeSystemGroup()
{
    InitializeComponent();
}

void Scenario4_ChangeSystemGroup::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
	create_task(JumpList::LoadCurrentAsync()).then([this](JumpList^ jumpList)
	{
		switch (jumpList->SystemGroupKind)
		{
		case JumpListSystemGroupKind::Recent:
			SystemGroup_Recent->IsChecked = true;
			break;
		case JumpListSystemGroupKind::Frequent:
			SystemGroup_Frequent->IsChecked = true;
			break;
		case JumpListSystemGroupKind::None:
			SystemGroup_None->IsChecked = true;
			break;
		}
	}, task_continuation_context::use_current());
}

void Scenario4_ChangeSystemGroup::SystemGroup_Recent_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
	create_task(JumpList::LoadCurrentAsync()).then([this](JumpList^ jumpList) -> IAsyncAction^
	{
		jumpList->SystemGroupKind = JumpListSystemGroupKind::Recent;
		return jumpList->SaveAsync();
	}, task_continuation_context::use_arbitrary());
}

void Scenario4_ChangeSystemGroup::SystemGroup_Frequent_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
	create_task(JumpList::LoadCurrentAsync()).then([this](JumpList^ jumpList) -> IAsyncAction^
	{
		jumpList->SystemGroupKind = JumpListSystemGroupKind::Frequent;
		return jumpList->SaveAsync();
	}, task_continuation_context::use_arbitrary());
}

void Scenario4_ChangeSystemGroup::SystemGroup_None_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
	create_task(JumpList::LoadCurrentAsync()).then([this](JumpList^ jumpList) -> IAsyncAction^
	{
		jumpList->SystemGroupKind = JumpListSystemGroupKind::None;
		return jumpList->SaveAsync();
	}, task_continuation_context::use_arbitrary());
}

void Scenario4_ChangeSystemGroup::PrepareSampleFiles_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    // NOTE: This is for the sake of the sample only. Real apps should allow
    // the system list to be populated by user activity.

	for (int i = 0; i < 5; ++i)
	{		
		create_task(ApplicationData::Current->LocalFolder->CreateFileAsync("Temp" + i.ToString() + ".ms-jumplist-sample")).then(
			[](task<StorageFile^> fileTask)
		{
			try
			{
				StorageFile^ file = fileTask.get();
				StorageApplicationPermissions::MostRecentlyUsedList->Add(file, "", RecentStorageItemVisibility::AppAndSystem);
			}
			catch (Exception^)
			{
				// It's ok that the file already exists. Just don't re-add it.
			}
		}, task_continuation_context::use_arbitrary());
    }
}