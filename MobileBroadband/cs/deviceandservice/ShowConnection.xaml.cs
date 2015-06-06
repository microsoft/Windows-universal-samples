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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using Windows.Networking.NetworkOperators;
using System.Collections.Generic;

namespace MobileBroadband
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ShowConnection : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private IReadOnlyList<string> deviceAccountId = null;

        public ShowConnection()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            PrepareScenario();
        }


        /// <summary>
        /// This is the click handler for the 'Default' button.  You would replace this with your own handler
        /// if you have a button or buttons on this page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ShowConnectionUI_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            if (b != null)
            {
                try
                {
                    var mobileBroadbandAccount = MobileBroadbandAccount.CreateFromNetworkAccountId(deviceAccountId[0]);
                    mobileBroadbandAccount.CurrentNetwork.ShowConnectionUI();
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser("Error:" + ex.Message, NotifyType.ErrorMessage);
                }
            }
        }


        void PrepareScenario()
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            try
            {
                deviceAccountId = MobileBroadbandAccount.AvailableNetworkAccountIds;

                if (deviceAccountId.Count != 0)
                {
                    ShowConnectionUI.Content = "Show Connection UI";
                    ShowConnectionUI.IsEnabled = true;
                }
                else
                {
                    ShowConnectionUI.Content = "No available accounts detected";
                    ShowConnectionUI.IsEnabled = false;
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Error:" + ex.Message, NotifyType.ErrorMessage);
            }
        }

    }
}
