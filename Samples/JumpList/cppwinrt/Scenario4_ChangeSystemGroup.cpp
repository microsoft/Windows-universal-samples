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
#include "Scenario4_ChangeSystemGroup.h"
#include "Scenario4_ChangeSystemGroup.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::AccessCache;
using namespace Windows::UI::StartScreen;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario4_ChangeSystemGroup::Scenario4_ChangeSystemGroup()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario4_ChangeSystemGroup::OnNavigatedTo(NavigationEventArgs const&)
    {
        auto lifetime = get_strong();

        JumpList jumpList = co_await JumpList::LoadCurrentAsync();
        switch (jumpList.SystemGroupKind())
        {
        case JumpListSystemGroupKind::Recent:
            SystemGroup_Recent().IsChecked(true);
            break;
        case JumpListSystemGroupKind::Frequent:
            SystemGroup_Frequent().IsChecked(true);
            break;
        case JumpListSystemGroupKind::None:
            SystemGroup_None().IsChecked(true);
            break;
        }
    }

    fire_and_forget Scenario4_ChangeSystemGroup::SystemGroup_Recent_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        JumpList jumpList = co_await JumpList::LoadCurrentAsync();
        jumpList.SystemGroupKind(JumpListSystemGroupKind::Recent);
        co_await jumpList.SaveAsync();
    }

    fire_and_forget Scenario4_ChangeSystemGroup::SystemGroup_Frequent_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        JumpList jumpList = co_await JumpList::LoadCurrentAsync();
        jumpList.SystemGroupKind(JumpListSystemGroupKind::Frequent);
        co_await jumpList.SaveAsync();
    }

    fire_and_forget Scenario4_ChangeSystemGroup::SystemGroup_None_Click(IInspectable const&, RoutedEventArgs const&)
    {
        JumpList jumpList = co_await JumpList::LoadCurrentAsync();
        jumpList.SystemGroupKind(JumpListSystemGroupKind::None);
        co_await jumpList.SaveAsync();
    }

    fire_and_forget Scenario4_ChangeSystemGroup::PrepareSampleFiles_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // NOTE: This is for the sake of the sample only. Real apps should allow
        // the system list to be populated by user activity.

        StorageFolder localFolder = ApplicationData::Current().LocalFolder();

        for (int i = 0; i < 5; ++i)
        {
            hstring fileName{ L"Temp" + std::to_wstring(i) + L".ms-jumplist-sample" };
            StorageFile file = co_await localFolder.CreateFileAsync(fileName, CreationCollisionOption::OpenIfExists);
            StorageApplicationPermissions::MostRecentlyUsedList().Add(file, L"", RecentStorageItemVisibility::AppAndSystem);
        }
    }
}
