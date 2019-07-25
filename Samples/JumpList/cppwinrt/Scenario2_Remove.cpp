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
#include "Scenario2_Remove.h"
#include "Scenario2_Remove.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::StartScreen;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Remove::Scenario2_Remove()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario2_Remove::RemoveJumpList_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        JumpList jumpList = co_await JumpList::LoadCurrentAsync();
        jumpList.SystemGroupKind(JumpListSystemGroupKind::None);
        jumpList.Items().Clear();
        co_await jumpList.SaveAsync();
    }
}
