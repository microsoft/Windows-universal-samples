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

using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario2_Launch : Page
    {
        public Scenario2_Launch()
        {
            this.InitializeComponent();
            LaunchInFullScreenModeCheckBox.IsChecked = ApplicationView.PreferredLaunchWindowingMode == ApplicationViewWindowingMode.FullScreen;
        }

        private void LaunchInFullScreenModeCheckBox_Click(object sender, RoutedEventArgs e)
        {
            ApplicationView.PreferredLaunchWindowingMode = LaunchInFullScreenModeCheckBox.IsChecked.Value ? ApplicationViewWindowingMode.FullScreen : ApplicationViewWindowingMode.Auto;
        }
    }
}
