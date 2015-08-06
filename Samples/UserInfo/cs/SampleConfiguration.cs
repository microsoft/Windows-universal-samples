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
using System.ComponentModel;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Account picture name C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Find users", ClassType = typeof(Scenario1_FindUsers) },
            new Scenario() { Title = "Watch users", ClassType = typeof(Scenario2_WatchUsers) },
        };
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
