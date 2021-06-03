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
#include "Scenario1_FindUsers.h"
#include "SampleConfiguration.h"
#include "Scenario1_FindUsers.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::System;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media::Imaging;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_FindUsers::Scenario1_FindUsers()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario1_FindUsers::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&)
    {
        auto lifetime = get_strong();

        // Populate the list of users.
        IVectorView<User> users = co_await User::FindAllAsync();
        auto observableUsers = single_threaded_observable_vector<UserViewModel>();
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
            observableUsers.Append({ user.NonRoamableId(), displayName });
        }
        UserList().DataContext(observableUsers);
        if (users.Size() > 0)
        {
            UserList().SelectedIndex(0);
        }
    }

    fire_and_forget Scenario1_FindUsers::ShowProperties()
    {
        auto lifetime = get_strong();

        auto selectedUser = UserList().SelectedValue().as<UserViewModel>();
        if (selectedUser != nullptr)
        {
            ResultsText().Text(L"");
            ProfileImage().Source(nullptr);
            rootPage.NotifyUser(L"", NotifyType::StatusMessage);
            try
            {
                User user = User::GetFromId(selectedUser.UserId());

                // Start with some fixed properties.
                hstring result = L"NonRoamableId: " + user.NonRoamableId ()+ L"\n"
                + L"Type: " + to_hstring(user.Type()) + L"\n";
                + L"AuthenticationStatus: " + to_hstring(user.AuthenticationStatus()) + L"\n";

                // Build a list of all the properties we want.
                IVector<hstring> desiredProperties = single_threaded_vector<hstring>(
                {
                    KnownUserProperties::FirstName(),
                    KnownUserProperties::LastName(),
                    KnownUserProperties::ProviderName(),
                    KnownUserProperties::AccountName(),
                    KnownUserProperties::GuestHost(),
                    KnownUserProperties::PrincipalName(),
                    KnownUserProperties::DomainName(),
                    KnownUserProperties::SessionInitiationProtocolUri(),
                });
                // Issue a bulk query for all of the properties.
                IPropertySet values = co_await user.GetPropertiesAsync(desiredProperties.GetView());

                // Add those properties to our results.
                for (auto property : desiredProperties)
                {
                    result = result + property + L": " + values.Lookup(property).as<hstring>() + L"\n";
                }
                ResultsText().Text(result);

                // Get the user's picture.
                IRandomAccessStreamReference streamReference = co_await user.GetPictureAsync(UserPictureSize::Size64x64);
                if (streamReference != nullptr)
                {
                    IRandomAccessStream stream = co_await streamReference.OpenReadAsync();
                    BitmapImage bitmapImage;
                    bitmapImage.SetSource(stream);
                    ProfileImage().Source(bitmapImage);
                }
            }
            catch (hresult_error const& ex)
            {
                rootPage.NotifyUser(ex.message(), NotifyType::ErrorMessage);
            }
        }
    }
}

