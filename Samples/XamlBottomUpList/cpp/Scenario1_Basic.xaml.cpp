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
#include "Scenario1_Basic.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;

Scenario1_Basic::Scenario1_Basic()
{
    InitializeComponent();

    // Start with one message, so the screen doesn't look empty.
    AddItemToEnd();
}

void Scenario1_Basic::AddItemToEnd()
{
    BottomUpList->Items->Append(Helpers::GetCurrentDateTimeAsString() + ": Message " + (++messageNumber).ToString());
}

void Scenario1_Basic::DeleteSelectedItem()
{
    int selectedIndex = BottomUpList->SelectedIndex;
    if (selectedIndex >= 0)
    {
        BottomUpList->Items->RemoveAt(static_cast<unsigned int>(selectedIndex));
        if (static_cast<unsigned int>(selectedIndex) < BottomUpList->Items->Size)
        {
            BottomUpList->SelectedIndex = selectedIndex;
        }
    }
}

void Scenario1_Basic::SaveScrollPosition()
{
    // We let the item be its own key. In a more general program, the item would have
    // a unique ID that identifies it in the list.
    savedScrollPosition = ListViewPersistenceHelper::GetRelativeScrollPosition(BottomUpList,
        ref new ListViewItemToKeyHandler([](Object^ o) { return o->ToString(); }));
}

void Scenario1_Basic::RestoreScrollPosition()
{
    if (savedScrollPosition != nullptr)
    {
        ListViewPersistenceHelper::SetRelativeScrollPositionAsync(BottomUpList, savedScrollPosition,
            ref new ListViewKeyToItemHandler([](String^ s) { return create_async([s]() { return task_from_result<Object^>(s); }); }));
    }
}
