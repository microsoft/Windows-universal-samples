//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario5_LaunchedFromSecondaryTile : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario5_LaunchedFromSecondaryTile()
        {
            this.InitializeComponent();

            if (!String.IsNullOrEmpty(rootPage.LaunchParam))
            {
                rootPage.NotifyUser("Application was activated from a Secondary Tile with the following Activation Arguments: " + MainPage.Current.LaunchParam, NotifyType.StatusMessage);
                rootPage.LaunchParam = null;
            }
        }
    }
}

