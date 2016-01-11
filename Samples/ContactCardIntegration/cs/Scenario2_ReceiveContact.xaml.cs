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
using Windows.ApplicationModel.Activation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// Page to handle the protocol activation. In this case we simply show the URI and the
    /// parameters passed to it.
    /// </summary>
    public sealed partial class Scenario2_ReceiveContact : Page
    {
        public Scenario2_ReceiveContact()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);

            var args = e.Parameter as ProtocolActivatedEventArgs;
            // Display the result of the protocol activation if we got here as a result of being activated for a protocol.
            if (args != null)
            {
                // Parse the URI to extract the protocol and the contact ids
                Uri uri = args.Uri;

                MainPage.Current.NotifyUser("Activated with protocol = " + uri.Scheme + ", parameters = " + uri.Query, NotifyType.StatusMessage);
            }
        }
    }
}
