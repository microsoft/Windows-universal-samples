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

using Windows.ApplicationModel.Activation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario4_ReceiveUri : Page
    {
        // A pointer back to the main page. This is needed if you want to call methods in MainPage such as NotifyUser()
        MainPage rootPage = MainPage.Current;

        string Protocol = "alsdkcs";

        public Scenario4_ReceiveUri()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached. The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var args = e.Parameter as ProtocolActivatedEventArgs;

            // Display the result of the protocol activation if we got here as a result of being activated for a protocol.
            if (args != null)
            {
                rootPage.NotifyUser("Protocol activation received. The received URI is " + args.Uri.AbsoluteUri + ".", NotifyType.StatusMessage);
            }
        }
    }
}