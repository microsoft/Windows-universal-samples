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
    public sealed partial class GetUserDisplayName : Page
    {
        private MainPage rootPage;

        public GetUserDisplayName()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private async void GetDisplayNameButton_Click(object sender, RoutedEventArgs e)
        {
            if (UserInformation.NameAccessAllowed)
            {
                string displayName = await UserInformation.GetDisplayNameAsync();
                if (string.IsNullOrEmpty(displayName))
                {
                    rootPage.NotifyUser("No display name was returned.", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Display name = " + displayName, NotifyType.StatusMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Access to name disabled by Privacy Setting or Group Policy.", NotifyType.ErrorMessage);
            }
        }
    }
}
