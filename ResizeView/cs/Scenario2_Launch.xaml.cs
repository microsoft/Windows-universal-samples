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

using Windows.Foundation;
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
            LaunchAtSize.IsChecked = ApplicationView.PreferredLaunchWindowingMode != ApplicationViewWindowingMode.Auto;
        }

        // If you set the PreferredLaunchViewSize and PreferredLaunchWindowingMode
        // before calling Window.Current.Activate(), then it will take effect
        // on the current launch. Otherwise, it takes effect at the next launch.
        private void LaunchAtSize_Click(object sender, RoutedEventArgs e)
        {
            if (LaunchAtSize.IsChecked.Value)
            {
                // For best results, set the PreferredLaunchViewSize before setting
                // the PreferredLaunchWindowingMode.
                ApplicationView.PreferredLaunchViewSize = new Size { Width = 600, Height = 500 };
                ApplicationView.PreferredLaunchWindowingMode = ApplicationViewWindowingMode.PreferredLaunchViewSize;
            }
            else
            {
                ApplicationView.PreferredLaunchWindowingMode = ApplicationViewWindowingMode.Auto;
            }
        }
    }
}
