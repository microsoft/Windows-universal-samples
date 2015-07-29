//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;

namespace AssociationLaunching
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ReceiveUri : Page
    {
        // A pointer back to the main page. This is needed if you want to call methods in MainPage such as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public ReceiveUri()
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
            // Show scenario description based on platform.
            if (Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
            {
                this.WindowsScenarioDescription.Visibility = Visibility.Collapsed;
            }
            else
                this.PhoneScenarioDescription.Visibility = Visibility.Collapsed;

            // Display the result of the protocol activation if we got here as a result of being activated for a protocol.
            if (rootPage.ProtocolEvent != null)
            {
                rootPage.NotifyUser("Protocol activation received. The received URI is " + rootPage.ProtocolEvent.Uri.AbsoluteUri + ".", NotifyType.StatusMessage);
            }
        }
    }
}