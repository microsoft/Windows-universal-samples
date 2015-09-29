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
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_WatchUsers sealed
    {
    public:
        Scenario2_WatchUsers();

        property Windows::Foundation::Collections::IVector<UserViewModel^>^ Users
        {
            Windows::Foundation::Collections::IVector<UserViewModel^>^ get()
            {
                return models;
            }
        }
    protected:
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        void StartWatching();
        void StopWatching();
    private:
        MainPage^ rootPage = MainPage::Current;
        Platform::Collections::Vector<UserViewModel^>^ models = ref new Platform::Collections::Vector<UserViewModel^>();
        int nextUserNumber = 1;
        Windows::System::UserWatcher^ userWatcher = nullptr;

        Windows::Foundation::EventRegistrationToken userAddedToken{};
        Windows::Foundation::EventRegistrationToken userUpdatedToken{};
        Windows::Foundation::EventRegistrationToken userRemovedToken{};
        Windows::Foundation::EventRegistrationToken enumerationCompletedToken{};
        Windows::Foundation::EventRegistrationToken watcherStoppedToken{};

        UserViewModel^ FindModelByUserId(Platform::String^ userId);
        concurrency::task<Platform::String^> GetDisplayNameOrGenericNameAsync(Windows::System::User^ user);
        void OnUserAdded(Windows::System::UserWatcher^ sender, Windows::System::UserChangedEventArgs^ e);
        void OnUserUpdated(Windows::System::UserWatcher^ sender, Windows::System::UserChangedEventArgs^ e);
        void OnUserRemoved(Windows::System::UserWatcher^ sender, Windows::System::UserChangedEventArgs^ e);
        void OnEnumerationCompleted(Windows::System::UserWatcher^ sender, Platform::Object^ e);
        void OnWatcherStopped(Windows::System::UserWatcher^ sender, Platform::Object^ e);
    };
}
