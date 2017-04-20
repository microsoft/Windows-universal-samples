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
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using Windows.Networking;
using Windows.System.RemoteSystems;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario4_Settings : Page
    {
        public Scenario4_Settings()
        {
            this.InitializeComponent();
        }

        private void CheckNow_Clicked(object sender, RoutedEventArgs e)
        {
            AnoymousAuthorizationEnabled.Text = RemoteSystem.IsAuthorizationKindEnabled(RemoteSystemAuthorizationKind.Anonymous).ToString();
            SameUserAuthorizationEnabled.Text = RemoteSystem.IsAuthorizationKindEnabled(RemoteSystemAuthorizationKind.SameUser).ToString();
        }
    }
}
