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
    public sealed partial class Scenario1_FindUsers : Page
    {
        private MainPage rootPage;

        public Scenario1_FindUsers()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            // Populate the list of users.
            IReadOnlyList<User> users = await User.FindAllAsync();
            var observableUsers = new ObservableCollection<UserViewModel>();
            int userNumber = 1;
            foreach (User user in users)
            {
                string displayName = (string)await user.GetPropertyAsync(KnownUserProperties.DisplayName);

                // Choose a generic name if we do not have access to the actual name.
                if (String.IsNullOrEmpty(displayName))
                {
                    displayName = "User #" + userNumber.ToString();
                    userNumber++;
                }
                observableUsers.Add(new UserViewModel(user.NonRoamableId, displayName));
            }
            UserList.DataContext = observableUsers;
            if (users.Count > 0)
            {
                UserList.SelectedIndex = 0;
            }
        }

        private async void ShowProperties()
        {
            var selectedUser = (UserViewModel)UserList.SelectedValue;
            if (selectedUser != null)
            {
                ResultsText.Text = "";
                ProfileImage.Source = null;
                rootPage.NotifyUser("", NotifyType.StatusMessage);
                try
                {
                    User user = User.GetFromId(selectedUser.UserId);

                    // Start with some fixed properties.
                    String result = "NonRoamableId: " + user.NonRoamableId + "\n";
                    result += "Type: " + user.Type.ToString() + "\n";
                    result += "AuthenticationStatus: " + user.AuthenticationStatus.ToString() + "\n";

                    // Build a list of all the properties we want.
                    String[] desiredProperties = new String[]
                    {
                        KnownUserProperties.FirstName,
                        KnownUserProperties.LastName,
                        KnownUserProperties.ProviderName,
                        KnownUserProperties.AccountName,
                        KnownUserProperties.GuestHost,
                        KnownUserProperties.PrincipalName,
                        KnownUserProperties.DomainName,
                        KnownUserProperties.SessionInitiationProtocolUri,
                    };
                    // Issue a bulk query for all of the properties.
                    IPropertySet values = await user.GetPropertiesAsync(desiredProperties);

                    // Add those properties to our results.
                    foreach (String property in desiredProperties)
                    {
                        result += property + ": " + values[property] + "\n";
                    }
                    ResultsText.Text = result;

                    // Get the user's picture.
                    IRandomAccessStreamReference streamReference = await user.GetPictureAsync(UserPictureSize.Size64x64);
                    if (streamReference != null)
                    {
                        IRandomAccessStream stream = await streamReference.OpenReadAsync();
                        BitmapImage bitmapImage = new BitmapImage();
                        bitmapImage.SetSource(stream);
                        ProfileImage.Source = bitmapImage;
                    }
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
                }
            }
        }
    }
}
