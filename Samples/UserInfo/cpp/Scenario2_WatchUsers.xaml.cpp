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
#include "Scenario2_WatchUsers.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_WatchUsers::Scenario2_WatchUsers()
{
    InitializeComponent();
    UserList->DataContext = models;
}

void Scenario2_WatchUsers::OnNavigatedFrom(NavigationEventArgs^ e)
{
    StopWatching();
}

void Scenario2_WatchUsers::StartWatching()
{
    rootPage->NotifyUser("", NotifyType::StatusMessage);

    nextUserNumber = 1;
    models->Clear();
    userWatcher = User::CreateWatcher();
    userAddedToken = userWatcher->Added += ref new TypedEventHandler<UserWatcher^, UserChangedEventArgs^>(this, &Scenario2_WatchUsers::OnUserAdded);
    userUpdatedToken = userWatcher->Updated += ref new TypedEventHandler<UserWatcher^, UserChangedEventArgs^>(this, &Scenario2_WatchUsers::OnUserUpdated);
    userRemovedToken = userWatcher->Removed += ref new TypedEventHandler<UserWatcher^, UserChangedEventArgs^>(this, &Scenario2_WatchUsers::OnUserRemoved);
    enumerationCompletedToken = userWatcher->EnumerationCompleted += ref new TypedEventHandler<UserWatcher^, Object^>(this, &Scenario2_WatchUsers::OnEnumerationCompleted);
    watcherStoppedToken = userWatcher->Stopped += ref new TypedEventHandler<UserWatcher^, Object^>(this, &Scenario2_WatchUsers::OnWatcherStopped);
    userWatcher->Start();
    StartButton->IsEnabled = false;
    StopButton->IsEnabled = true;
}

void Scenario2_WatchUsers::StopWatching()
{
    if (userWatcher != nullptr)
    {
        // Unregister all event handlers in case events are in flight.
        userWatcher->Added -= userAddedToken;
        userWatcher->Updated -= userUpdatedToken;
        userWatcher->Removed -= userRemovedToken;
        userWatcher->EnumerationCompleted -= enumerationCompletedToken;
        userWatcher->Stopped -= watcherStoppedToken;
        userWatcher->Stop();
        userWatcher = nullptr;
        models->Clear();
        StartButton->IsEnabled = true;
        StopButton->IsEnabled = false;
    }
}

UserViewModel^ Scenario2_WatchUsers::FindModelByUserId(String^ userId)
{
    for (UserViewModel^ model : models)
    {
        if (model->UserId == userId)
        {
            return model;
        }
    }
    return nullptr;
}

task<String^> Scenario2_WatchUsers::GetDisplayNameOrGenericNameAsync(User^ user)
{
    // Try to get the display name.
    auto task = create_task(user->GetPropertyAsync(KnownUserProperties::DisplayName));
    return task.then([this](Object^ result)
    {
        auto displayName = safe_cast<String^>(result);
        if (displayName->IsEmpty())
        {
            displayName = "User #" + nextUserNumber.ToString();
            ++nextUserNumber;
        }
        return displayName;
    });
}

void Scenario2_WatchUsers::OnUserAdded(UserWatcher^ sender, UserChangedEventArgs^ e)
{
    User^ user = e->User;

    // UI work must happen on the UI thread.
    rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
        [this, user]()
    {
        // Create the user with "..." as the temporary display name.
        // Add it right away, because it might get removed while the
        // "await GetDisplayNameOrGenericNameAsync()" is running.
        auto model = ref new UserViewModel(user->NonRoamableId, L"\u2026");
        models->Append(model);

        // Try to get the display name.
        GetDisplayNameOrGenericNameAsync(user).then([model](String^ displayName)
        {
            model->DisplayName = displayName;
        });
    }));
}

void Scenario2_WatchUsers::OnUserUpdated(UserWatcher^ sender, UserChangedEventArgs^ e)
{
    User^ user = e->User;

    // UI work must happen on the UI thread.
    rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
        [this, user]()
    {
        // Look for the user in our collection and update the display name.
        UserViewModel^ model = FindModelByUserId(user->NonRoamableId);
        if (model != nullptr)
        {
            GetDisplayNameOrGenericNameAsync(user).then([model](String^ displayName)
            {
                model->DisplayName = displayName;
            });
        }
    }));
}

void Scenario2_WatchUsers::OnUserRemoved(UserWatcher^ sender, UserChangedEventArgs^ e)
{
    User^ user = e->User;

    // UI work must happen on the UI thread.
    rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
        [this, user]()
    {
        // Look for the user in our collection and remove it.
        UserViewModel^ model = FindModelByUserId(user->NonRoamableId);
        if (model != nullptr)
        {
            unsigned int index;
            if (models->IndexOf(model, &index))
            {
                models->RemoveAt(index);
            }
        }
    }));
}

void Scenario2_WatchUsers::OnEnumerationCompleted(UserWatcher^ sender, Object^ e)
{
    // UI work must happen on the UI thread.
    rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
        [this]()
    {
        rootPage->NotifyUser("Enumeration complete. Watching for changes...", NotifyType::StatusMessage);
    }));
}

void Scenario2_WatchUsers::OnWatcherStopped(UserWatcher^ sender, Object^ e)
{
    // UI work must happen on the UI thread.
    rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
        [this]()
    {
        StopWatching();
    }));

}
