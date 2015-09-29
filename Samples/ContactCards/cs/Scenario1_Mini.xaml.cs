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

using Windows.ApplicationModel.Contacts;
using Windows.Foundation;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Mini : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_Mini()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (!ContactManager.IsShowContactCardSupported())
            {
                NotSupportedWarning.Visibility = Visibility.Visible;
            }
        }

        private void ShowContactCard_Click(object sender, RoutedEventArgs e)
        {
            Contact contact = rootPage.CreateContactFromUserInput(EmailAddress, PhoneNumber);
            if (contact != null)
            {
                // Show the contact card next to the button.
                Rect rect = MainPage.GetElementRect(sender as FrameworkElement);

                // Show with default placement.
                ContactManager.ShowContactCard(contact, rect);
            }
        }

        private void ShowContactCardWithPlacement_Click(object sender, RoutedEventArgs e)
        {
            Contact contact = rootPage.CreateContactFromUserInput(EmailAddress, PhoneNumber);
            if (contact != null)
            {
                // Show the contact card next to the button.
                Rect rect = MainPage.GetElementRect(sender as FrameworkElement);

                // Show with preferred placement to the right.
                ContactManager.ShowContactCard(contact, rect, Placement.Right);
            }
        }

        private void ShowContactCardWithOptions_Click(object sender, RoutedEventArgs e)
        {
            Contact contact = rootPage.CreateContactFromUserInput(EmailAddress, PhoneNumber);
            if (contact != null)
            {
                // Show the contact card next to the button.
                Rect rect = MainPage.GetElementRect(sender as FrameworkElement);

                // Ask for the initial tab to be Phone.
                ContactCardOptions options = new ContactCardOptions() { InitialTabKind = ContactCardTabKind.Phone };

                // Show with default placement.
                ContactManager.ShowContactCard(contact, rect, Placement.Default, options);
            }
        }

    }
}
