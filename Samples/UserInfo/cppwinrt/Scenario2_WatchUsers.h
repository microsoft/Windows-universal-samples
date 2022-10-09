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

#pragma once
#include "Scenario2_WatchUsers.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_WatchUsers : Scenario2_WatchUsersT<Scenario2_WatchUsers>
    {
        Scenario2_WatchUsers();

        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

        Windows::Foundation::Collections::IObservableVector<SDKTemplate::UserViewModel> Users()
        {
            return users;
        }

        void StartWatching();
        void StopWatching();

    private:
        fire_and_forget OnUserAdded(Windows::System::UserWatcher const&, Windows::System::UserChangedEventArgs const& e);
        fire_and_forget OnUserUpdated(Windows::System::UserWatcher const&, Windows::System::UserChangedEventArgs const& e);
        fire_and_forget OnUserRemoved(Windows::System::UserWatcher const&, Windows::System::UserChangedEventArgs const& e);
        fire_and_forget OnEnumerationCompleted(Windows::System::UserWatcher const&, IInspectable const& e);
        fire_and_forget OnWatcherStopped(Windows::System::UserWatcher const&, IInspectable const& e);

        SDKTemplate::UserViewModel FindModelByUserId(hstring const& userId);
        Windows::Foundation::IAsyncOperation<hstring> GetDisplayNameOrGenericNameAsync(Windows::System::User user);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };

        Windows::Foundation::Collections::IObservableVector<SDKTemplate::UserViewModel> users = single_threaded_observable_vector<SDKTemplate::UserViewModel>();
        Windows::System::UserWatcher userWatcher{ nullptr };
        event_token userAddedToken{};
        event_token userUpdatedToken{};
        event_token userRemovedToken{};
        event_token enumerationCompletedToken{};
        event_token watcherStoppedToken{};

        int userNumber = 1;
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_WatchUsers : Scenario2_WatchUsersT<Scenario2_WatchUsers, implementation::Scenario2_WatchUsers>
    {
    };
}
