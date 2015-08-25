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
using System.Threading.Tasks;
using System.Text;
using Windows.ApplicationModel.Contacts;
using Windows.Foundation;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Single : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_Single()
        {
            this.InitializeComponent();
        }

        private void ClearOutputs()
        {
            // Reset the output fields.
            OutputText.Text = "";
            rootPage.NotifyUser("", NotifyType.StatusMessage);
        }

        private void ReportContactResult(Contact contact)
        {
            if (contact != null)
            {
                OutputText.Text = MainPage.GetContactResult(contact);
            }
            else
            {
                rootPage.NotifyUser("No contact was selected.", NotifyType.ErrorMessage);
            }
        }

        private async void PickContactEmail()
        {
            ClearOutputs();

            // Ask the user to pick a contact email address.
            var contactPicker = new ContactPicker();
            contactPicker.DesiredFieldsWithContactFieldType.Add(ContactFieldType.Email);

            Contact contact = await contactPicker.PickContactAsync();

            ReportContactResult(contact);
        }

        private async void PickContactPhone()
        {
            ClearOutputs();

            // Ask the user to pick a contact phone number.
            var contactPicker = new ContactPicker();
            contactPicker.DesiredFieldsWithContactFieldType.Add(ContactFieldType.PhoneNumber);

            Contact contact = await contactPicker.PickContactAsync();

            ReportContactResult(contact);
        }
    }

}
