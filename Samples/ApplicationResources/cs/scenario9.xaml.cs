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
using System.Collections.Generic;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario9 : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario9()
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
        }

        /// <summary>
        /// Invoked when the user has selected a different scenario and navigation away from
        /// this page is about to occur.
        /// </summary>
        /// <param name="e">Event data that describes the navigation that has unloaded this
        /// page.</param>
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            // Clearing qualifier value overrides set on the default context for the current
            // view so that an override set here doesn't impact other scenarios. See comments
            // below for additional information.
            ResourceContext.GetForCurrentView().Reset();
            base.OnNavigatedFrom(e);
        }

        /// <summary>
        /// This is the click handler for the 'Scenario9Button_Show' button.  You would replace this with your own handler
        /// if you have a button or buttons on this page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Scenario9Button_Show_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            if (b != null)
            {
                // A langauge override will be set on the default context for the current view.
                // When navigating between different scenarios in this sample, the content for each
                // scenario is loaded into a host page that remains constant. The view (meaning, the 
                // CoreWindow) remains the same, and so it is the same default context that's in use. 
                // Thus, an override set here can impact behavior in other scenarios.
                //
                // However, the description for the scenario may give the impression that a value 
                // being set is local to this scenario. Also, when entering this scenario, the combo 
                // box always gets set to the first item, which can add to the confusion. To avoid 
                // confusion when using this sample, the override that gets set here will be cleared 
                // when the user navigates away from this scenario (in the OnNavigatedFrom event 
                // handler). In a real app, whether and when to clear an override will depend on
                // the desired behaviour and the design of the app.

                var context = ResourceContext.GetForCurrentView();

                var selectedLanguage = Scenario9ComboBox.SelectedValue;
                if (selectedLanguage != null)
                {
                    var lang = new List<string>();
                    lang.Add(selectedLanguage.ToString());
                    context.Languages = lang;
                    var resourceStringMap = ResourceManager.Current.MainResourceMap.GetSubtree("Resources");
                    this.Scenario9TextBlock.Text = resourceStringMap.GetValue("string1", context).ValueAsString;
                }
            }
        }

    }
}
