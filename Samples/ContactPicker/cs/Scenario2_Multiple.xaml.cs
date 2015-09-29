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

using System;
using System.Collections.Generic;
using Windows.ApplicationModel.Contacts;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2_Multiple : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario2_Multiple()
        {
            this.InitializeComponent();
        }

        private void ClearOutputs()
        {
            // Reset the output fields.
            OutputText.Text = "";
            rootPage.NotifyUser("", NotifyType.StatusMessage);
        }

        private void ReportContactResults(IList<Contact> contacts)
        {
            if (contacts != null && contacts.Count > 0)
            {
                foreach (Contact contact in contacts)
                {
                    OutputText.Text += MainPage.GetContactResult(contact) + "\n";
                }
            }
            else
            {
                rootPage.NotifyUser("No contacts were selected.", NotifyType.ErrorMessage);
            }
        }


        private async void PickContactsEmail()
        {
            ClearOutputs();

            var contactPicker = new ContactPicker();
            // Ask the user to pick contact email addresses.
            contactPicker.DesiredFieldsWithContactFieldType.Add(ContactFieldType.Email);
            var contacts = await contactPicker.PickContactsAsync();

            ReportContactResults(contacts);
        }

        private async void PickContactsPhone()
        {
            ClearOutputs();

            var contactPicker = new ContactPicker();
            // Ask the user to pick contact phone numbers.
            contactPicker.DesiredFieldsWithContactFieldType.Add(ContactFieldType.PhoneNumber);
            var contacts = await contactPicker.PickContactsAsync();

            ReportContactResults(contacts);
        }

    }
}
