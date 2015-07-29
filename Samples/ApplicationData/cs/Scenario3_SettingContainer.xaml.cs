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
using SDKTemplate;
using System;
using Windows.Storage;

namespace ApplicationDataSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class SettingContainer : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        ApplicationDataContainer localSettings = null;

        const string containerName = "exampleContainer";
        const string settingName = "exampleSetting";

        public SettingContainer()
        {
            this.InitializeComponent();

            localSettings = ApplicationData.Current.LocalSettings;

            DisplayOutput();
        }

        void CreateContainer_Click(Object sender, RoutedEventArgs e)
        {
            ApplicationDataContainer container = localSettings.CreateContainer(containerName, ApplicationDataCreateDisposition.Always);

            DisplayOutput();
        }

        void DeleteContainer_Click(Object sender, RoutedEventArgs e)
        {
            localSettings.DeleteContainer(containerName);

            DisplayOutput();
        }

        void WriteSetting_Click(Object sender, RoutedEventArgs e)
        {
            if (localSettings.Containers.ContainsKey(containerName))
            {
                localSettings.Containers[containerName].Values[settingName] = "Hello World"; // example value
            }

            DisplayOutput();
        }

        void DeleteSetting_Click(Object sender, RoutedEventArgs e)
        {
            if (localSettings.Containers.ContainsKey(containerName))
            {
                localSettings.Containers[containerName].Values.Remove(settingName);
            }

            DisplayOutput();
        }

        void DisplayOutput()
        {
            bool hasContainer = localSettings.Containers.ContainsKey(containerName);
            bool hasSetting = hasContainer ? localSettings.Containers[containerName].Values.ContainsKey(settingName) : false;

            String output = String.Format("Container Exists: {0}\n" +
                                          "Setting Exists: {1}",
                                          hasContainer ? "true" : "false",
                                          hasSetting ? "true" : "false");

            OutputTextBlock.Text = output;
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }
    }
}
