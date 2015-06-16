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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using System;
using Windows.System.UserProfile;

namespace SDKTemplate
{
    public sealed partial class GetUserFirstAndLastName : Page
    {
        private MainPage rootPage;

        public GetUserFirstAndLastName()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private async void GetFirstNameButton_Click(object sender, RoutedEventArgs e)
        {
            if (UserInformation.NameAccessAllowed)
            {
                string firstName = await UserInformation.GetFirstNameAsync();
                if (string.IsNullOrEmpty(firstName))
                {
                    rootPage.NotifyUser("No first name was returned.", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("First name = " + firstName, NotifyType.StatusMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Access to name disabled by Privacy Setting or Group Policy.", NotifyType.ErrorMessage);
            }
        }

        private async void GetLastNameButton_Click(object sender, RoutedEventArgs e)
        {
            if (UserInformation.NameAccessAllowed)
            {
                string lastName = await UserInformation.GetLastNameAsync();
                if (string.IsNullOrEmpty(lastName))
                {
                    rootPage.NotifyUser("No last name was returned.", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Last name = " + lastName, NotifyType.StatusMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Access to name disabled by Privacy Setting or Group Policy.", NotifyType.ErrorMessage);
            }
        }
    }
}
