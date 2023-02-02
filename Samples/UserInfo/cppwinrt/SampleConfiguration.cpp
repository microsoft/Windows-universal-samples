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
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "UserViewModel.h"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::System;
using namespace winrt::SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"User info C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Find users", xaml_typename<SDKTemplate::Scenario1_FindUsers>() },
    Scenario{ L"Watch users", xaml_typename<SDKTemplate::Scenario2_WatchUsers>() },
    Scenario{ L"Check user consent group", xaml_typename<SDKTemplate::Scenario3_CheckUserConsentGroup>() },
});

hstring winrt::to_hstring(UserType value)
{
    switch (value)
    {
    case UserType::LocalUser: return L"LocalUser";
    case UserType::RemoteUser: return L"RemoteUser";
    case UserType::LocalGuest: return L"LocalGuest";
    case UserType::RemoteGuest: return L"RemoteGuest";
    }
    return to_hstring(static_cast<int32_t>(value));
}

hstring winrt::to_hstring(UserAuthenticationStatus value)
{
    switch (value)
    {
    case UserAuthenticationStatus::Unauthenticated: return L"Unauthenticated";
    case UserAuthenticationStatus::LocallyAuthenticated: return L"LocallyAuthenticated";
    case UserAuthenticationStatus::RemotelyAuthenticated: return L"RemotelyAuthenticated";
    }
    return to_hstring(static_cast<int32_t>(value));
}

IAsyncOperation<IObservableVector<IInspectable>> winrt::SDKTemplate::Helpers::GetUserViewModelsAsync()
{
    // Populate the list of users.
    IVectorView<User> users = co_await User::FindAllAsync();
    std::vector<IInspectable> usersVector;
    int userNumber = 1;
    for (auto&& user : users)
    {
        hstring displayName = unbox_value<hstring>(co_await user.GetPropertyAsync(KnownUserProperties::DisplayName()));

        // Choose a generic name if we do not have access to the actual name.
        if (displayName.empty())
        {
            displayName = L"User #" + to_hstring(userNumber);
            userNumber++;
        }
        usersVector.emplace_back(make<implementation::UserViewModel>(user.NonRoamableId(), displayName));
    }

    co_return single_threaded_observable_vector(std::move(usersVector));
}
