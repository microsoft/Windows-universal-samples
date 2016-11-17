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
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario6 : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario6()
        {
            this.InitializeComponent();
        }

        private async void pfnLink_Click(object sender, RoutedEventArgs e)
        {            
            Uri uri = new Uri("bingmaps:?rtp=adr.Mountain%20View,%20CA~adr.San%20Francisco,%20CA&amp;mode=d&amp;trfc=1");
            var launcherOptions = new LauncherOptions();
            launcherOptions.TargetApplicationPackageFamilyName = "Microsoft.WindowsMaps_8wekyb3d8bbwe";
            await Launcher.LaunchUriAsync(uri, launcherOptions);
        }
    }
}
