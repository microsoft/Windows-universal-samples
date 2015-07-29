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

using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using Windows.Devices.Radios;
using System;

namespace RadioManagerSample
{
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage;

        public Scenario1()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            // RequestAccessAsync must be called at least once from the UI thread
            var accessLevel = await Radio.RequestAccessAsync();
            if (accessLevel != RadioAccessStatus.Allowed)
            {
                rootPage.NotifyUser("App is not allowed to control radios.", NotifyType.ErrorMessage);
            }
            else
            {
                InitializeRadios();
            }
        }

        private async void InitializeRadios()
        {
            // An alternative to Radio.GetRadiosAsync is to use the Windows.Devices.Enumeration pattern,
            // passing Radio.GetDeviceSelector as the AQS string
            var radios = await Radio.GetRadiosAsync();
            foreach (var radio in radios)
            {
                RadioSwitchList.Items.Add(new RadioModel(radio, this));
            }
        }
    }
}
