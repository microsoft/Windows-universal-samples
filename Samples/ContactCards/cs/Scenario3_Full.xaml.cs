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
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3_Full : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario3_Full()
        {
            this.InitializeComponent();
        }

        private void ShowContactCard_Click(object sender, RoutedEventArgs e)
        {
            Contact contact = rootPage.CreateContactFromUserInput(EmailAddress, PhoneNumber);
            if (contact != null)
            {
                // Try to share the screen half/half with the full contact card.
                FullContactCardOptions options = new FullContactCardOptions();
                options.DesiredRemainingView = ViewSizePreference.UseHalf;

                // Show the full contact card.
                ContactManager.ShowFullContactCard(contact, options);
            }
        }
    }
}
