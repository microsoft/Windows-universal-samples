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
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario3_SettingContainer : Page
    {
        MainPage rootPage = MainPage.Current;
        ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;

        const string containerName = "exampleContainer";
        const string settingName = "exampleSetting";

        public Scenario3_SettingContainer()
        {
            InitializeComponent();
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
            localSettings.Containers[containerName].Values[settingName] = "Hello World"; // example value

            DisplayOutput();
        }

        void DeleteSetting_Click(Object sender, RoutedEventArgs e)
        {
            localSettings.Containers[containerName].Values.Remove(settingName);

            DisplayOutput();
        }

        void DisplayOutput()
        {
            bool hasContainer = localSettings.Containers.TryGetValue(containerName, out ApplicationDataContainer container);
            bool hasSetting = hasContainer ? container.Values.ContainsKey(settingName) : false;

            OutputTextBlock.Text = $"Container Exists: {hasContainer}\nSetting Exists: {hasSetting}";

            WriteSetting.IsEnabled = hasContainer;
            DeleteSetting.IsEnabled = hasContainer;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            DisplayOutput();
        }
    }
}
