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
#include "Scenario2_Remove.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::StartScreen;

Scenario2_Remove::Scenario2_Remove()
{
    InitializeComponent();
}

void Scenario2_Remove::RemoveJumpList_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
	create_task(JumpList::LoadCurrentAsync()).then([this](JumpList^ jumpList) -> IAsyncAction^
	{
		jumpList->SystemGroupKind = JumpListSystemGroupKind::None;
		jumpList->Items->Clear();
		return jumpList->SaveAsync();
	}, task_continuation_context::use_arbitrary());
}
