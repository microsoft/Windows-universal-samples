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
using System.Globalization;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Devices.WiFiDirect.Services;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    namespace WiFiDirectServices
    {
        /// <summary>
        /// UI that exposes properties to begin advertising a Wi-Fi Direct Service.
        /// A normal application would hide this implementation and just provide a way to begin advertising a service
        ///
        /// Most of the actual logic for Wi-Fi Direct Services is in WiFiDirectServicesManager.cs and WiFiDirectServicesWrappers.cs
        /// </summary>
        public sealed partial class Scenario1 : Page
        {
            private MainPage rootPage;

            public Scenario1()
            {
                this.InitializeComponent();
            }
            
            protected override void OnNavigatedTo(NavigationEventArgs e)
            {
                rootPage = MainPage.Current;

                ResetFields();
                WiFiDirectServiceManager.Instance.Scenario1 = this;
            }

            protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
            {
                WiFiDirectServiceManager.Instance.Scenario1 = null;
            }

            private void ResetFields()
            {
                ServiceName.Text = "";
                AutoAccept.IsChecked = true;
                PreferGO.IsChecked = true;
                Status.SelectedIndex = 0;
                CustomStatus.Text = "";
                ConfigMethodDefault.IsChecked = true;
                ConfigMethodDispaly.IsChecked = true;
                ConfigMethodDisplayPin.Text = "";
                ConfigMethodKeypad.IsChecked = false;
                DeferredSessionInfo.Text = "";
                ServiceInformation.Text = "";
                ServicePrefixList.Items.Clear();
            }

            private void CreateAdvertiser_Click(object sender, RoutedEventArgs e)
            {
                rootPage.NotifyUser("Creating Advertisement", NotifyType.StatusMessage);

                try
                {
                    List<WiFiDirectServiceConfigurationMethod> configMethods = new List<WiFiDirectServiceConfigurationMethod>();

                    if (ConfigMethodDispaly.IsChecked ?? false)
                    {
                        configMethods.Add(WiFiDirectServiceConfigurationMethod.PinDisplay);
                    }
                    if (ConfigMethodKeypad.IsChecked ?? false)
                    {
                        configMethods.Add(WiFiDirectServiceConfigurationMethod.PinEntry);
                    }
                    if (ConfigMethodDefault.IsChecked ?? false)
                    {
                        configMethods.Add(WiFiDirectServiceConfigurationMethod.Default);
                    }

                    WiFiDirectServiceStatus status = WiFiDirectServiceStatus.Available;
                    if (Status.SelectedItem.ToString() == "Busy")
                    {
                        status = WiFiDirectServiceStatus.Busy;
                    }
                    else if (Status.SelectedItem.ToString() == "Custom")
                    {
                        status = WiFiDirectServiceStatus.Custom;
                    }

                    uint customStatus = 0;
                    if (CustomStatus.Text.Length > 0)
                    {
                        uint temp = 0;
                        if (uint.TryParse(CustomStatus.Text, out temp))
                        {
                            customStatus = temp;
                        }
                    }

                    // NOTE: Each prefix published will be handled by the driver so there are limitations
                    // to how many prefixes can be supported. In general, apps should only publish prefixes
                    // of the service name that are required for discovery
                    // Typically, this would be something like "com.example.games" for a service called "com.example.games.foobar"
                    IList<String> prefixList = new List<String>();
                    foreach (var prefix in ServicePrefixList.Items)
                    {
                        // NOTE: these must be actual prefixes of the service name
                        // Publication will fail if they are not
                        prefixList.Add((string)prefix);
                    }

                    WiFiDirectServiceManager.Instance.StartAdvertisement(
                        ServiceName.Text,
                        AutoAccept.IsChecked ?? false,
                        PreferGO.IsChecked ?? false,
                        ConfigMethodDisplayPin.Text,
                        configMethods,
                        status,
                        customStatus,
                        ServiceInformation.Text,
                        DeferredSessionInfo.Text,
                        prefixList
                        );

                    ResetFields();

                    // Force navigation to the next page for smoother experience
                    rootPage.GoToScenario(1);
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser(String.Format(CultureInfo.InvariantCulture, "Failed to start service: {0}", ex.Message), NotifyType.ErrorMessage);
                }
            }

            private void RemovePrefix_Click(object sender, RoutedEventArgs e)
            {
                foreach (var prefix in ServicePrefixList.SelectedItems)
                {
                    ServicePrefixList.Items.Remove(prefix);
                }
            }

            private void AddPrefix_Click(object sender, RoutedEventArgs e)
            {
                if (NewPrefix.Text != "")
                {
                    ServicePrefixList.Items.Add(NewPrefix.Text);
                    NewPrefix.Text = "";
                }
            }
        }
    }
}
