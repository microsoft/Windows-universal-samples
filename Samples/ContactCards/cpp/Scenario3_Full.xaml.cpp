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
#include "Scenario3_Full.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::ApplicationModel::Contacts;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario3_Full::Scenario3_Full()
{
    this->InitializeComponent();
}

void Scenario3_Full::ShowContactCard_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    Contact^ contact = rootPage->CreateContactFromUserInput(EmailAddress, PhoneNumber);
    if (contact != nullptr)
    {
        // Try to share the screen half/half with the full contact card.
        FullContactCardOptions^ options = ref new FullContactCardOptions();
        options->DesiredRemainingView = ViewSizePreference::UseHalf;

        // Show the full contact card.
        ContactManager::ShowFullContactCard(contact, options);
    }
}
