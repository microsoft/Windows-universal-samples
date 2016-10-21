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
#include "Scenario2_DelayMini.xaml.h"

using namespace Concurrency;
using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::ApplicationModel::Contacts;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario2_DelayMini::Scenario2_DelayMini()
{
    InitializeComponent();
}

void Scenario2_DelayMini::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    if (!ContactManager::IsShowDelayLoadedContactCardSupported())
    {
        NotSupportedWarning->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
}

Contact^ Scenario2_DelayMini::CreatePlaceholderContact()
{
    // Create contact object with small set of initial data to display.
    Contact^ contact = ref new Contact();
    contact->FirstName = "Kim";
    contact->LastName = "Abercrombie";

    ContactEmail^ email = ref new ContactEmail();
    email->Address = "kim@contoso.com";
    contact->Emails->Append(email);

    return contact;
}

task<Contact^> Scenario2_DelayMini::DownloadContactDataAsync(Contact^ contact)
{
    bool downloadSucceeded = DownloadSucceeded->IsChecked->Value;

    return create_task([this, contact, downloadSucceeded]()
    {
        // Simulate the download latency by delaying the execution by 2 seconds.
        Sleep(2000);

        if (!downloadSucceeded)
        {
            return static_cast<Contact^>(nullptr);
        }

        // Add more data to the contact object.
        ContactEmail^ workEmail = ref new ContactEmail();
        workEmail->Address = "kim@example.com";
        workEmail->Kind = ContactEmailKind::Work;
        contact->Emails->Append(workEmail);

        ContactPhone^ homePhone = ref new ContactPhone();
        homePhone->Number = "(444) 555-0101";
        homePhone->Kind = ContactPhoneKind::Home;
        contact->Phones->Append(homePhone);

        ContactPhone^ workPhone = ref new ContactPhone();
        workPhone->Number = "(245) 555-0123";
        workPhone->Kind = ContactPhoneKind::Work;
        contact->Phones->Append(workPhone);

        ContactPhone^ mobilePhone = ref new ContactPhone();
        mobilePhone->Number = "(921) 555-0187";
        mobilePhone->Kind = ContactPhoneKind::Mobile;
        contact->Phones->Append(mobilePhone);

        ContactAddress^ address = ref new ContactAddress();
        address->StreetAddress = "123 Main St";
        address->Locality = "Redmond";
        address->Region = "WA";
        address->Country = "USA";
        address->PostalCode = "00000";
        address->Kind = ContactAddressKind::Home;
        contact->Addresses->Append(address);

        return contact;
    });
}

void Scenario2_DelayMini::ShowContactCard_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    Contact^ contact = CreatePlaceholderContact();

    // Show the contact card next to the button.
    Rect rect = MainPage::GetElementRect(safe_cast<FrameworkElement^>(sender));

    // The contact card placement can change when it is updated with more data. For improved user experience, specify placement 
    // of the card so that it has space to grow and will not need to be repositioned. In this case, default placement first places 
    // the card above the button because the card is small, but after the card is updated with more data, the operating system moves 
    // the card below the button to fit the card's expanded size. Specifying that the contact card is placed below at the beginning 
    // avoids this repositioning.
    Placement placement = Placement::Below;

    // For demonstration purposes, we ask for the Enterprise contact card.
    ContactCardOptions^ options = ref new ContactCardOptions();
    options->HeaderKind = ContactCardHeaderKind::Enterprise;

    ContactCardDelayedDataLoader^ dataLoader = ContactManager::ShowDelayLoadedContactCard(contact, rect, placement, options);

    if (dataLoader != nullptr)
    {
        // Simulate downloading more data from the network for the contact.
        this->rootPage->NotifyUser("Simulating download...", NotifyType::StatusMessage);

        DownloadContactDataAsync(contact).then([this, dataLoader](Contact^ fullContact)
        {
            String^ message;
            if (fullContact != nullptr)
            {
                // Update the contact card with the full set of contact data.
                dataLoader->SetData(fullContact);
                message = "Contact has been updated with downloaded data.";
            }
            else
            {
                message = "No further information available.";
            }

            Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message]()
            {
                this->rootPage->NotifyUser(message, NotifyType::StatusMessage);
            }));

            // Dispose the object to indicate that the delay-loading operation has completed.
            delete dataLoader;
        });
    }
    else
    {
        this->rootPage->NotifyUser("ShowDelayLoadedContactCard is not supported by this device.", NotifyType::ErrorMessage);
    }
}
