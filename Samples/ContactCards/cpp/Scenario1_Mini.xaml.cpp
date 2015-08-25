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
#include "Scenario1_Mini.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::ApplicationModel::Contacts;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;

Scenario1_Mini::Scenario1_Mini()
{
    InitializeComponent();
}

void Scenario1_Mini::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    if (!ContactManager::IsShowContactCardSupported())
    {
        NotSupportedWarning->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
}

void Scenario1_Mini::ShowContactCard_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    Contact^ contact = rootPage->CreateContactFromUserInput(EmailAddress, PhoneNumber);
    if (contact != nullptr)
    {
        // Show the contact card next to the button.
        Rect rect = MainPage::GetElementRect(safe_cast<FrameworkElement^>(sender));

        // Show with default placement.
        ContactManager::ShowContactCard(contact, rect);
    }
}

void Scenario1_Mini::ShowContactCardWithPlacement_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    Contact^ contact = rootPage->CreateContactFromUserInput(EmailAddress, PhoneNumber);
    if (contact != nullptr)
    {
        // Show the contact card next to the button.
        Rect rect = MainPage::GetElementRect(safe_cast<FrameworkElement^>(sender));

        // Show with preferred placement to the right.
        ContactManager::ShowContactCard(contact, rect, Placement::Right);
    }
}

void Scenario1_Mini::ShowContactCardWithOptions_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    Contact^ contact = rootPage->CreateContactFromUserInput(EmailAddress, PhoneNumber);
    if (contact != nullptr)
    {
        // Show the contact card next to the button.
        Rect rect = MainPage::GetElementRect(safe_cast<FrameworkElement^>(sender));

        // Ask for the initial tab to be Phone.
        ContactCardOptions^ options = ref new ContactCardOptions();
        options->InitialTabKind = ContactCardTabKind::Phone;

        // Show with default placement.
        ContactManager::ShowContactCard(contact, rect, Placement::Default);
    }
}
