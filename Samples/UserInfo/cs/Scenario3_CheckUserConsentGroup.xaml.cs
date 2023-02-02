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
using System.Collections.ObjectModel;
using Windows.Foundation.Collections;
using Windows.Storage.Streams;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3_CheckUserConsentGroup : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario3_CheckUserConsentGroup()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            // Populate the list of users.
            var observableUsers = await MainPage.GetUserViewModelsAsync();
            UserList.DataContext = observableUsers;
            if (observableUsers.Count > 0)
            {
                UserList.SelectedIndex = 0;
            }
        }

        static string EvaluateConsentResult(UserAgeConsentResult consentResult)
        {
            switch (consentResult)
            {
                case UserAgeConsentResult.Included:
                case UserAgeConsentResult.NotEnforced:
                    return "Allowed.";
                case UserAgeConsentResult.NotIncluded:
                    return "Not allowed.";
                case UserAgeConsentResult.Unknown:
                    return "Cannot determine. Default to app specific age unknown behavior.";
                default:
                case UserAgeConsentResult.Ambiguous:
                    return "Age regulations have changed, the app needs to be updated to reflect new catagories.";
            }
        }

        private async void ShowConsentGroups(object sender, RoutedEventArgs e)
        {
            var selectedUser = (UserViewModel)UserList.SelectedValue;
            if (selectedUser != null)
            {
                ResultsText.Text = "";

                rootPage.NotifyUser("", NotifyType.StatusMessage);
                try
                {
                    User user = User.GetFromId(selectedUser.UserId);

                    String result = "";

                    UserAgeConsentResult canShowChildContent = await user.CheckUserAgeConsentGroupAsync(UserAgeConsentGroup.Child);
                    result += "Child content: " + EvaluateConsentResult(canShowChildContent) + "\n";

                    UserAgeConsentResult canShowMinorContent = await user.CheckUserAgeConsentGroupAsync(UserAgeConsentGroup.Minor);
                    result += "Minor content: " + EvaluateConsentResult(canShowMinorContent) + "\n";

                    UserAgeConsentResult canShowAdultContent = await user.CheckUserAgeConsentGroupAsync(UserAgeConsentGroup.Adult);
                    result += "Adult content: " + EvaluateConsentResult(canShowAdultContent) + "\n";

                    ResultsText.Text = result;
                }
                catch (Exception ex)
                {
                    // Operations on the "User" object fail if the user signs out.
                    rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
                }
            }
        }
    }
}
