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
using System.Diagnostics;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.ApplicationModel.Background;
using Windows.Foundation.Collections;
using Windows.Devices.WiFi;
using Windows.Security.Credentials;
using Windows.Storage;

namespace SDKTemplate
{
    public sealed partial class Scenario1_ManageHotspot : Page
    {
        WiFiOnDemandHotspotNetwork m_network;
        MainPage rootPage = MainPage.Current;

        public Scenario1_ManageHotspot()
        {
            this.InitializeComponent();
        }

        static bool IsBackgroundTaskName(string name)
        {
            return name == Constants.MetadataUpdateTaskName ||
                name == Constants.DeviceWatcherTaskName ||
                name == Constants.ConnectTaskName;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            m_network = WiFiOnDemandHotspotNetwork.GetOrCreateById(Constants.SampleHotspotGuid);

            // Copy the values from the network properties into our UI.
            WiFiOnDemandHotspotNetworkProperties properties = m_network.GetProperties();
            DisplayNameText.Text = properties.DisplayName;
            AvailableToggle.IsOn = properties.Availability == WiFiOnDemandHotspotAvailability.Available;
            WiFiOnDemandHotspotCellularBars? bars = properties.CellularBars;
            CellularBarsToggle.IsOn = bars.HasValue;
            Debug.Assert(0 == (int)WiFiOnDemandHotspotCellularBars.ZeroBars);
            Debug.Assert(1 == (int)WiFiOnDemandHotspotCellularBars.OneBar);
            Debug.Assert(2 == (int)WiFiOnDemandHotspotCellularBars.TwoBars);
            Debug.Assert(3 == (int)WiFiOnDemandHotspotCellularBars.ThreeBars);
            Debug.Assert(4 == (int)WiFiOnDemandHotspotCellularBars.FourBars);
            Debug.Assert(5 == (int)WiFiOnDemandHotspotCellularBars.FiveBars);
            CellularBarsSlider.Value = (int)(bars ?? WiFiOnDemandHotspotCellularBars.ZeroBars);
            uint? batteryPercent = properties.RemainingBatteryPercent;
            BatteryPercentageToggle.IsOn = batteryPercent.HasValue;
            BatteryPercentageSlider.Value = (int)(batteryPercent ?? 0);
            SsidText.Text = properties.Ssid;
            PasswordText.Password = properties.Password?.Password ?? String.Empty;

            // Determine whether the background tasks are registered.
            bool isRegistered = false;
            foreach (var cur in BackgroundTaskRegistration.AllTasks)
            {
                if (IsBackgroundTaskName(cur.Value.Name))
                {
                    isRegistered = true;
                    break;
                }
            }

            VisualStateManager.GoToState(this, isRegistered ? "Registered" : "NotRegistered", false);
        }

        private void RegisterTasks_Click(object sender, RoutedEventArgs e)
        {
            // Register the background task which updates the hotspot metadata when the system
            // shows a list of Wi-Fi networks.
            var updateMetadataTrigger = new WiFiOnDemandHotspotUpdateMetadataTrigger();
            var backgroundTaskBuilder = new BackgroundTaskBuilder();
            backgroundTaskBuilder.Name = Constants.MetadataUpdateTaskName;
            backgroundTaskBuilder.TaskEntryPoint = Constants.MetadataUpdateEntryPoint;
            backgroundTaskBuilder.SetTrigger(updateMetadataTrigger);
            BackgroundTaskRegistration updateMetadataRegistration = backgroundTaskBuilder.Register();

            // Register the background task which turns on the on-demand hotspot when the user
            // selects it.
            var connectTrigger = new WiFiOnDemandHotspotConnectTrigger();
            backgroundTaskBuilder.Name = Constants.ConnectTaskName;
            backgroundTaskBuilder.TaskEntryPoint = Constants.ConnectEntryPoint;
            backgroundTaskBuilder.SetTrigger(connectTrigger);
            BackgroundTaskRegistration connectRegistration = backgroundTaskBuilder.Register();

            // In practice, you will monitor when an on-demand hotspot comes into range or goes
            // of range so you can update the Availability. This sample does not have a physical
            // hotspot device, but we leave the code here as a demonstration.
#if false
            DeviceWatcher deviceWatcher = DeviceInformation.CreateWatcher(Contoso.Devices.OnDemandHotSpot.GetDeviceSelector());
            var triggerEventKinds = new List<DeviceWatcherEventKind> { DeviceWatcherEventKind.Add, DeviceWatcherEventKind.Remove, DeviceWatcherEventKind.Update };
            DeviceWatcherTrigger watcherTrigger = deviceWatcher.GetBackgroundTrigger(triggerEventKinds);
            backgroundTaskBuilder.Name = Constants.DeviceWatcherTaskName;
            backgroundTaskBuilder.TaskEntryPoint = Constants.DeviceWatcherEntryPoint;
            backgroundTaskBuilder.SetTrigger(watcherTrigger);
            BackgroundTaskRegistration deviceWatcherRegistration = backgroundTaskBuilder.Register();
#endif

            VisualStateManager.GoToState(this, "Registered", false);
        }

        private void UnregisterTasks_Click(object sender, RoutedEventArgs e)
        {
            foreach (var cur in BackgroundTaskRegistration.AllTasks)
            {
                if (IsBackgroundTaskName(cur.Value.Name))
                {
                    cur.Value.Unregister(true);
                }
            }

            VisualStateManager.GoToState(this, "NotRegistered", false);
        }

        private bool SavePropertiesForBackgroundTask()
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            if (AvailableToggle.IsOn && String.IsNullOrEmpty(DisplayNameText.Text))
            {
                rootPage.NotifyUser("An Available network must have a name.", NotifyType.ErrorMessage);
                return false;
            }

            if (String.IsNullOrEmpty(SsidText.Text))
            {
                rootPage.NotifyUser("You must provide an SSID for the Wi-Fi network.", NotifyType.ErrorMessage);
                return false;
            }

            if (String.IsNullOrEmpty(PasswordText.Password))
            {
                rootPage.NotifyUser("You must provide a password for the Wi-Fi network.", NotifyType.ErrorMessage);
                return false;
            }


            IPropertySet values = ApplicationData.Current.LocalSettings.Values;
            values["availability"] = AvailableToggle.IsOn;
            values["displayName"] = DisplayNameText.Text;
            values["bars"] = CellularBarsToggle.IsOn ? (int?)CellularBarsSlider.Value : null;
            values["battery"] = BatteryPercentageToggle.IsOn ? (uint?)BatteryPercentageSlider.Value : null;
            values["ssid"] = SsidText.Text;
            values["password"] = PasswordText.Password;
            values["hotspotGuid"] = Constants.SampleHotspotGuid;

            return true;
        }

        private void UpdateNowButton_Click(object sender, RoutedEventArgs e)
        {
            if (SavePropertiesForBackgroundTask())
            {
                // Copy the properties from the UI into the system network properties
                // to configure how the on-demand hotspot appears in the system Wi-Fi list.
                WiFiOnDemandHotspotNetworkProperties properties = m_network.GetProperties();
                properties.Availability = AvailableToggle.IsOn ? WiFiOnDemandHotspotAvailability.Available : WiFiOnDemandHotspotAvailability.Unavailable;
                properties.DisplayName = DisplayNameText.Text;
                properties.CellularBars = CellularBarsToggle.IsOn ? (WiFiOnDemandHotspotCellularBars?)(int?)CellularBarsSlider.Value : null;
                properties.RemainingBatteryPercent = BatteryPercentageToggle.IsOn ? (uint?)BatteryPercentageSlider.Value : null;
                properties.Ssid = SsidText.Text;
                properties.Password = new PasswordCredential { Password = PasswordText.Password };
                m_network.UpdateProperties(properties);
            }
        }

        private void UpdateOnDemandButton_Click(object sender, RoutedEventArgs e)
        {
            SavePropertiesForBackgroundTask();
        }
    }
}

