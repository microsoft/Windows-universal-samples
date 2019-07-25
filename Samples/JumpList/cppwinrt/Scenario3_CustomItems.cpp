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
#include "Scenario3_CustomItems.h"
#include "Scenario3_CustomItems.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::StartScreen;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_CustomItems::Scenario3_CustomItems()
    {
        InitializeComponent();
    }

    void Scenario3_CustomItems::DisplayNameLocalizedResource_Click(IInspectable const&, RoutedEventArgs const&)
    {
        DisplayName().IsEnabled(!DisplayNameLocalizedResource().IsChecked());
        DisplayName().Text (L"ms-resource:///Resources/CustomJumpListItemDisplayName");

    }

    void Scenario3_CustomItems::DescriptionLocalizedResource_Click(IInspectable const&, RoutedEventArgs const&)
    {
        Description().IsEnabled(!DescriptionLocalizedResource().IsChecked());
        Description().Text(L"ms-resource:///Resources/CustomJumpListItemDescription");

    }

    void Scenario3_CustomItems::GroupNameLocalizedResource_Click(IInspectable const&, RoutedEventArgs const&)
    {
        GroupName().IsEnabled(!GroupNameLocalizedResource().IsChecked());
        GroupName().Text(L"ms-resource:///Resources/CustomJumpListGroupName");
    }

    fire_and_forget Scenario3_CustomItems::AddToJumpList_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        JumpList jumpList = co_await JumpList::LoadCurrentAsync();
        jumpList.SystemGroupKind(JumpListSystemGroupKind::Recent);

        auto item = JumpListItem::CreateWithArguments(Arguments().Text(), DisplayName().Text());
        item.Description(Description().Text());
        item.GroupName(GroupName().Text());
        item.Logo(Uri(L"ms-appx:///Assets/smalltile-sdk.png"));

        jumpList.Items().Append(item);

        co_await jumpList.SaveAsync();
    }
}
