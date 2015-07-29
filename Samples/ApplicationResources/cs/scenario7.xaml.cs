//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
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
using Windows.ApplicationModel.Resources.Core;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario7 : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        // pointer to default resource context for this view
        ResourceContext defaultContextForCurrentView;


        public Scenario7()
        {
            this.InitializeComponent();
            defaultContextForCurrentView = ResourceContext.GetForCurrentView();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            this.Scenario7TextBlock.Text = "";
        }

        /// <summary>
        /// This is the click handler for the 'Scenario7Button_Show' button.  You would replace this with your own handler
        /// if you have a button or buttons on this page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Scenario7Button_Show_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            if (b != null)
            {

                this.Scenario7TextBlock.Text = ResourceManager.Current.MainResourceMap.GetValue("Resources/string1", defaultContextForCurrentView).ValueAsString;

                defaultContextForCurrentView.QualifierValues.MapChanged += async (s, m) =>
                {
                    await this.Scenario7TextBlock.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                    {
                        this.Scenario7TextBlock.Text = ResourceManager.Current.MainResourceMap.GetValue("Resources/string1", defaultContextForCurrentView).ValueAsString;
                    });
                };
            }
        }

    }
}
