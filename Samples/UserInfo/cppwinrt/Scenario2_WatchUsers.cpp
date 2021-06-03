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
#include "Scenario2_WatchUsers.h"
#include "Scenario2_WatchUsers.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::System;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_WatchUsers::Scenario2_WatchUsers()
    {
        InitializeComponent();
    }

    void Scenario2_WatchUsers::OnNavigatedFrom(NavigationEventArgs const&)
    {
        StopWatching();
    }

    void Scenario2_WatchUsers::StartWatching()
    {
        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        userNumber = 1;
        users.Clear();
        userWatcher = User::CreateWatcher();
        
        userAddedToken = userWatcher.Added({ get_weak(), &Scenario2_WatchUsers::OnUserAdded });
        userUpdatedToken = userWatcher.Updated({ get_weak(), &Scenario2_WatchUsers::OnUserUpdated });
        userRemovedToken = userWatcher.Removed({ get_weak(), &Scenario2_WatchUsers::OnUserRemoved });
        enumerationCompletedToken = userWatcher.EnumerationCompleted({ get_weak(), &Scenario2_WatchUsers::OnEnumerationCompleted });
        watcherStoppedToken = userWatcher.Stopped({ get_weak(), &Scenario2_WatchUsers::OnWatcherStopped });
        userWatcher.Start();
        StartButton().IsEnabled(false);
        StopButton().IsEnabled(true);
    }

    void Scenario2_WatchUsers::StopWatching()
    {
        if (userWatcher != nullptr)
        {
            // Unregister all event handlers in case events are in flight.
            userWatcher.Added(userAddedToken);
            userWatcher.Updated(userUpdatedToken);
            userWatcher.Removed(userRemovedToken);
            userWatcher.EnumerationCompleted(enumerationCompletedToken);
            userWatcher.Stopped(watcherStoppedToken);
            userWatcher.Stop();
            userWatcher = nullptr;
            users.Clear();
            StartButton().IsEnabled(true);
            StopButton().IsEnabled(false);
        }
    }

    SDKTemplate::UserViewModel Scenario2_WatchUsers::FindModelByUserId(hstring const& userId)
    {
        for (UserViewModel model : users)
        {
            if (model.UserId() == userId)
            {
                return model;
            }
        }
        return nullptr;
    }

    IAsyncOperation<hstring> Scenario2_WatchUsers::GetDisplayNameOrGenericNameAsync(User user)
    {
        auto lifetime = get_strong();

        // Try to get the display name.
        hstring displayName = (co_await user.GetPropertyAsync(KnownUserProperties::DisplayName())).as<hstring>();

        // Choose a generic name if we do not have access to the actual name.
        if (displayName.empty())
        {
            displayName = L"User #" + to_hstring(userNumber);
            userNumber++;
        }
        return displayName;
    }

    fire_and_forget Scenario2_WatchUsers::OnUserAdded(UserWatcher const&, UserChangedEventArgs const& e)
    {
        auto lifetime = get_strong();

        User user = e.User();

        // UI work must happen on the UI thread.
        co_await resume_foreground(Dispatcher());

        // Create the user with "..." as the temporary display name.
        // Add it right away, because it might get removed while the
        // "co_await GetDisplayNameOrGenericNameAsync()" is running.
        UserViewModel model(user.NonRoamableId(), L"\u2026");
        users.Append(model);

        // Try to get the display name.
        model.DisplayName(co_await GetDisplayNameOrGenericNameAsync(user));
    }

    fire_and_forget Scenario2_WatchUsers::OnUserUpdated(UserWatcher const&, UserChangedEventArgs const& e)
    {
        auto lifetime = get_strong();

        User user = e.User();

        // UI work must happen on the UI thread.
        co_await resume_foreground(Dispatcher());

        // Look for the user in our collection and update the display name.
        UserViewModel model = FindModelByUserId(user.NonRoamableId());
        if (model != nullptr)
        {
            model.DisplayName(co_await GetDisplayNameOrGenericNameAsync(user));
        }
    }

    fire_and_forget Scenario2_WatchUsers::OnUserRemoved(UserWatcher const&, UserChangedEventArgs const& e)
    {
        auto lifetime = get_strong();

        User user = e.User();

        // UI work must happen on the UI thread.
        co_await resume_foreground(Dispatcher());

        // Look for the user in our collection and remove it.
        UserViewModel model = FindModelByUserId(user.NonRoamableId());
        if (model != nullptr)
        {
            uint32_t index;
            if (users.IndexOf(model, index))
            {
                users.RemoveAt(index);
            }
        }
    }

    fire_and_forget Scenario2_WatchUsers::OnEnumerationCompleted(UserWatcher const&, IInspectable const&)
    {
        auto lifetime = get_strong();

        // UI work must happen on the UI thread.
        co_await resume_foreground(Dispatcher());

        rootPage.NotifyUser(L"Enumeration complete. Watching for changes...", NotifyType::StatusMessage);
    }

    fire_and_forget Scenario2_WatchUsers::OnWatcherStopped(UserWatcher const&, IInspectable const&)
    {
        auto lifetime = get_strong();

        // UI work must happen on the UI thread.
        co_await resume_foreground(Dispatcher());

        StopWatching();
    }



}
