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
#include "Scenario3_CheckUserConsentGroup.h"
#include "SampleConfiguration.h"
#include "Scenario3_CheckUserConsentGroup.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::System;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media::Imaging;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_CheckUserConsentGroup::Scenario3_CheckUserConsentGroup()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario3_CheckUserConsentGroup::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&)
    {
        auto lifetime = get_strong();

        auto users = co_await Helpers::GetUserViewModelsAsync();
        UserList().DataContext(users);
        if (users.Size() > 0)
        {
            UserList().SelectedIndex(0);
        }
    }

    hstring Scenario3_CheckUserConsentGroup::EvaluateConsentResult(UserAgeConsentResult consentResult)
    {
        switch (consentResult)
        {
        case UserAgeConsentResult::Included:
        case UserAgeConsentResult::NotEnforced:
            return L"Allowed.";
        case UserAgeConsentResult::NotIncluded:
            return L"Not allowed.";
        case UserAgeConsentResult::Unknown:
            return L"Cannot determine. Default to app specific age unknown behavior.";
        default:
        case UserAgeConsentResult::Ambiguous:
            return L"Age regulations have changed, the app needs to be updated to reflect new catagories.";
        }
    }

    fire_and_forget Scenario3_CheckUserConsentGroup::ShowConsentGroups(IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        auto selectedUser = UserList().SelectedValue().as<SDKTemplate::UserViewModel>();
        if (selectedUser != nullptr)
        {
            ResultsText().Text(L"");
            rootPage.NotifyUser(L"", NotifyType::StatusMessage);
            try
            {
                User user = User::GetFromId(selectedUser.UserId());

                UserAgeConsentResult canShowChildContent = co_await user.CheckUserAgeConsentGroupAsync(UserAgeConsentGroup::Child);
                hstring result = L"Child content: " + EvaluateConsentResult(canShowChildContent) + L"\n";

                UserAgeConsentResult canShowMinorContent = co_await user.CheckUserAgeConsentGroupAsync(UserAgeConsentGroup::Minor);
                result = result + L"Minor content: " + EvaluateConsentResult(canShowMinorContent) + L"\n";

                UserAgeConsentResult canShowAdultContent = co_await user.CheckUserAgeConsentGroupAsync(UserAgeConsentGroup::Adult);
                result = result + L"Adult content: " + EvaluateConsentResult(canShowAdultContent) + L"\n";

                ResultsText().Text(result);
            }
            catch (hresult_error const& ex)
            {
                // Operations on the "User" object fail if the user signs out.
                rootPage.NotifyUser(ex.message(), NotifyType::ErrorMessage);
            }
        }
    }
}

