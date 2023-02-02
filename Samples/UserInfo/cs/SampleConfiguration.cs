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
using System.ComponentModel;
using System.Threading.Tasks;
using Windows.System;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Account picture name C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Find users", ClassType = typeof(Scenario1_FindUsers) },
            new Scenario() { Title = "Watch users", ClassType = typeof(Scenario2_WatchUsers) },
            new Scenario() { Title = "Check user consent group", ClassType = typeof(Scenario3_CheckUserConsentGroup) },
        };

        public static async Task<ObservableCollection<UserViewModel>> GetUserViewModelsAsync()
        {
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
            return observableUsers;
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    // This is an observable object that we data-bind the combo box / list box to.
    public class UserViewModel : INotifyPropertyChanged
    {
        public UserViewModel(String userId, String displayName)
        {
            this.UserId = userId;
            this.displayName = displayName;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public String UserId { get; private set; }
        public String DisplayName
        {
            get
            {
                return displayName;
            }
            set
            {
                displayName = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("DisplayName"));
                }
            }
        }

        String displayName;
    }
}
