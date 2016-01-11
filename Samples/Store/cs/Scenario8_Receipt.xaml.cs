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
using System.Threading.Tasks;
using System.Xml.Linq;
using Windows.ApplicationModel.Store;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// A page for eighth scenario.
    /// </summary>
    public sealed partial class Scenario8_Receipt : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser().
        MainPage rootPage = MainPage.Current;

        public Scenario8_Receipt()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            await MainPage.ConfigureSimulatorAsync("receipt.xml");
        }

        private async void ShowReceipt()
        {
            try
            {
                String receipt = await CurrentAppSimulator.GetAppReceiptAsync();
                String prettyReceipt = XElement.Parse(receipt).ToString(SaveOptions.None);
                Output.Text = prettyReceipt;
            }
            catch (Exception)
            {
                rootPage.NotifyUser("Get Receipt failed.", NotifyType.ErrorMessage);
            }
        }
    }
}
