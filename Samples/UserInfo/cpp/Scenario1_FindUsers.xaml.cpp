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
#include "Scenario1_FindUsers.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::System;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_FindUsers::Scenario1_FindUsers()
{
    InitializeComponent();
    UserList->DataContext = models;
}

void Scenario1_FindUsers::OnNavigatedTo(NavigationEventArgs^ e)
{
    // Populate the list of users.
    create_task(User::FindAllAsync()).then([this](IVectorView<User^>^ users)
    {
        // Get the display name of each user.
        std::vector<task<Object^>> tasks(users->Size);
        std::transform(begin(users), end(users), begin(tasks), [](User^ user)
        {
            return create_task(user->GetPropertyAsync(KnownUserProperties::DisplayName));
        });
        return when_all(begin(tasks), end(tasks)).then([this, users](std::vector<Object^> results)
        {
            // For each user and display name...
            for (size_t index = 0; index < users->Size; index++)
            {
                auto displayName = safe_cast<String^>(results[index]);
                // Choose a generic name if we do not have access to the actual name.
                if (displayName->IsEmpty())
                {
                    displayName = "User #" + nextUserNumber.ToString();
                    nextUserNumber++;
                }

                // Add it to our collection.
                models->Append(ref new UserViewModel(users->GetAt(index)->NonRoamableId, displayName));
            }
            if (users->Size > 0)
            {
                UserList->SelectedIndex = 0;
            }
        });
    });
}

void Scenario1_FindUsers::ShowProperties()
{
    auto model = safe_cast<UserViewModel^>(UserList->SelectedValue);
    if (model != nullptr)
    {
        ResultsText->Text = "";
        ProfileImage->Source = nullptr;
        rootPage->NotifyUser("", NotifyType::StatusMessage);

        User^ user = nullptr;
        try
        {
            user = User::GetFromId(model->UserId);
        }
        catch (Exception^ ex)
        {
        }

        if (user != nullptr)
        {
            auto desiredProperties = ref new Vector<String^>();
            desiredProperties->Append(KnownUserProperties::FirstName);
            desiredProperties->Append(KnownUserProperties::LastName);
            desiredProperties->Append(KnownUserProperties::ProviderName);
            desiredProperties->Append(KnownUserProperties::AccountName);
            desiredProperties->Append(KnownUserProperties::GuestHost);
            desiredProperties->Append(KnownUserProperties::PrincipalName);
            desiredProperties->Append(KnownUserProperties::DomainName);
            desiredProperties->Append(KnownUserProperties::SessionInitiationProtocolUri);

            // Issue a bulk query for all of the properties above.
            auto task = create_task(user->GetPropertiesAsync(desiredProperties->GetView()));
            task.then([this, desiredProperties, user](IPropertySet^ values)
            {
                // Generate the results.
                String^ result = "NonRoamableId: " + user->NonRoamableId + "\n";
                result += "Type: " + user->Type.ToString() + "\n";
                result += "AuthenticationStatus: " + user->AuthenticationStatus.ToString() + "\n";

                for (String^ propertyName : desiredProperties)
                {
                    result += propertyName + ": " + values->Lookup(propertyName) + "\n";
                }

                ResultsText->Text = result;

                // Get the user's picture.
                return create_task(user->GetPictureAsync(UserPictureSize::Size64x64));
            }).then([this](IRandomAccessStreamReference^ streamReference)
            {
                if (streamReference != nullptr)
                {
                    return create_task(streamReference->OpenReadAsync());
                }
                else
                {
                    return create_task([]() -> IRandomAccessStreamWithContentType^ { return nullptr; });
                }
            }).then([this](IRandomAccessStreamWithContentType^ stream)
            {
                if (stream != nullptr)
                {
                    auto bitmapImage = ref new BitmapImage();
                    bitmapImage->SetSource(stream);
                    ProfileImage->Source = bitmapImage;
                }
            });
        }
        else
        {
            rootPage->NotifyUser("Could not reacquire the user", NotifyType::ErrorMessage);
        }
    }
}
