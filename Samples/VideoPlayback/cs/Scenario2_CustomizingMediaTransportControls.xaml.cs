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
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// This scenario demonstrates how to customize the media transport controls.
    /// </summary>
    /// <remarks>
    /// See related code in CustomMediaTransportControls.cs and generic.xaml.
    /// </remarks>
    public sealed partial class Scenario2 : Page
    {
        private MainPage rootPage = MainPage.Current;
        private int likes = 0;

        public Scenario2()
        {
            this.InitializeComponent();
        }

        private void CustomMTC_Liked(object sender, EventArgs e)
        {
            rootPage.NotifyUser("You liked this video " + (++likes) + " times.", NotifyType.StatusMessage);
        }
    }
}
