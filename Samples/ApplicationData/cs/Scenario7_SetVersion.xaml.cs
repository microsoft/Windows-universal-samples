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

namespace SDKTemplate
{
    public sealed partial class Scenario7_SetVersion : Page
    {
        ApplicationData appData = ApplicationData.Current;

        const string settingName = "SetVersionSetting";
        const string settingValue0 = "Data.v0";
        const string settingValue1 = "Data.v1";

        public Scenario7_SetVersion()
        {
            InitializeComponent();
        }

        void SetVersionHandler0(SetVersionRequest request)
        {
            SetVersionDeferral deferral = request.GetDeferral();

            uint version = appData.Version;

            switch (version)
            {
                case 0:
                    // Version is already 0.  Nothing to do.
                    break;

                case 1:
                    // Need to convert data from v1 to v0.

                    // This sample simulates that conversion by writing a version-specific value.
                    appData.LocalSettings.Values[settingName] = settingValue0;

                    break;

                default:
                    throw new Exception("Unexpected ApplicationData Version: " + version);
            }

            deferral.Complete();
        }

        void SetVersionHandler1(SetVersionRequest request)
        {
            SetVersionDeferral deferral = request.GetDeferral();

            uint version = appData.Version;

            switch (version)
            {
                case 0:
                    // Need to convert data from v0 to v1.

                    // This sample simulates that conversion by writing a version-specific value.
                    appData.LocalSettings.Values[settingName] = settingValue1;

                    break;

                case 1:
                    // Version is already 1.  Nothing to do.
                    break;

                default:
                    throw new Exception("Unexpected ApplicationData Version: " + version);
            }

            deferral.Complete();
        }

        async void SetVersion0_Click(Object sender, RoutedEventArgs e)
        {
            await appData.SetVersionAsync(0, SetVersionHandler0);
            DisplayOutput();
        }

        async void SetVersion1_Click(Object sender, RoutedEventArgs e)
        {
            await appData.SetVersionAsync(1, SetVersionHandler1);
            DisplayOutput();
        }

        void DisplayOutput()
        {
            OutputTextBlock.Text = "Version: " + appData.Version;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            DisplayOutput();
        }
    }
}
