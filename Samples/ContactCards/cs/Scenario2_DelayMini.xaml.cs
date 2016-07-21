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

using System.Threading.Tasks;
using Windows.ApplicationModel.Contacts;
using Windows.Foundation;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2_DelayMini : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario2_DelayMini()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (!ContactManager.IsShowDelayLoadedContactCardSupported())
            {
                NotSupportedWarning.Visibility = Visibility.Visible;
            }
        }

        private static Contact CreatePlaceholderContact()
        {
            // Create contact object with small set of initial data to display.
            Contact contact = new Contact();
            contact.FirstName = "Kim";
            contact.LastName = "Abercrombie";

            ContactEmail email = new ContactEmail();
            email.Address = "kim@contoso.com";
            contact.Emails.Add(email);

            return contact;
        }

        private async Task<Contact> DownloadContactDataAsync(Contact contact)
        {
            // Simulate the download latency by delaying the execution by 2 seconds.
            await Task.Delay(2000);

            if (!DownloadSucceeded.IsChecked.Value)
            {
                return null;
            }

            // Add more data to the contact object.
            ContactEmail workEmail = new ContactEmail();
            workEmail.Address = "kim@example.com";
            workEmail.Kind = ContactEmailKind.Work;
            contact.Emails.Add(workEmail);

            ContactPhone homePhone = new ContactPhone();
            homePhone.Number = "(444) 555-0101";
            homePhone.Kind = ContactPhoneKind.Home;
            contact.Phones.Add(homePhone);

            ContactPhone workPhone = new ContactPhone();
            workPhone.Number = "(245) 555-0123";
            workPhone.Kind = ContactPhoneKind.Work;
            contact.Phones.Add(workPhone);

            ContactPhone mobilePhone = new ContactPhone();
            mobilePhone.Number = "(921) 555-0187";
            mobilePhone.Kind = ContactPhoneKind.Mobile;
            contact.Phones.Add(mobilePhone);

            ContactAddress address = new ContactAddress();
            address.StreetAddress = "123 Main St";
            address.Locality = "Redmond";
            address.Region = "WA";
            address.Country = "USA";
            address.PostalCode = "00000";
            address.Kind = ContactAddressKind.Home;
            contact.Addresses.Add(address);

            return contact;
        }

        private async void ShowContactCard_Click(object sender, RoutedEventArgs e)
        {
            Contact contact = CreatePlaceholderContact();

            // Show the contact card next to the button.
            Rect rect = MainPage.GetElementRect(sender as FrameworkElement);

            // The contact card placement can change when it is updated with more data. For improved user experience, specify placement 
            // of the card so that it has space to grow and will not need to be repositioned. In this case, default placement first places 
            // the card above the button because the card is small, but after the card is updated with more data, the operating system moves 
            // the card below the button to fit the card's expanded size. Specifying that the contact card is placed below at the beginning 
            // avoids this repositioning.
            Placement placement = Placement.Below;

            // For demonstration purposes, we ask for the Enterprise contact card.
            ContactCardOptions options = new ContactCardOptions() { HeaderKind = ContactCardHeaderKind.Enterprise };

            using (ContactCardDelayedDataLoader dataLoader = ContactManager.ShowDelayLoadedContactCard(contact, rect, placement, options))
            {
                if (dataLoader != null)
                {
                    // Simulate downloading more data from the network for the contact.
                    this.rootPage.NotifyUser("Simulating download...", NotifyType.StatusMessage);

                    Contact fullContact = await DownloadContactDataAsync(contact);
                    if (fullContact != null)
                    {
                        // Update the contact card with the full set of contact data.
                        dataLoader.SetData(fullContact);
                        this.rootPage.NotifyUser("Contact has been updated with downloaded data.", NotifyType.StatusMessage);
                    }
                    else
                    {
                        this.rootPage.NotifyUser("No further information available.", NotifyType.StatusMessage);
                    }
                }
                else
                {
                    this.rootPage.NotifyUser("ShowDelayLoadedContactCard is not supported by this device.", NotifyType.ErrorMessage);
                }
                // The "using" statement will dispose the dataLoader for us.
            }
        }
    }
}
