//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
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
using Windows.Storage;

namespace SDKTemplate
{
    public sealed partial class Scenario2_Settings : Page
    {
        ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;

        const string SettingName = "exampleSetting";

        public Scenario2_Settings()
        {
            InitializeComponent();
        }

        // Guidance for Settings.
        //
        // Settings are a convenient way of storing small pieces of configuration data
        // for your application.
        //
        // There are two storage containers for settings: Local and Roaming. However,
        // Roaming settings no longer roam, so they are functionally equivalent to Local.
        //
        // Care should be taken to guard against an excessive volume of data being
        // stored in settings.  Settings are not intended to be used as a database.
        // Large data sets will take longer to load from disk during your application's
        // launch.

        // This sample illustrates reading and writing from a local setting.

        void WriteSetting_Click(Object sender, RoutedEventArgs e)
        {
            localSettings.Values[SettingName] = "Hello World"; // example value

            DisplayOutput();
        }

        void DeleteSetting_Click(Object sender, RoutedEventArgs e)
        {
            localSettings.Values.Remove(SettingName);

            DisplayOutput();
        }

        void DisplayOutput()
        {
            Object value = localSettings.Values[SettingName];

            OutputTextBlock.Text = String.Format("Setting: {0}", (value == null ? "<empty>" : ("\"" + value + "\"")));
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            DisplayOutput();
        }
    }
}
