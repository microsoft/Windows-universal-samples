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
using Windows.Storage;

namespace ApplicationDataSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class CompositeSettings : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        ApplicationDataContainer roamingSettings = null;

        const string settingName = "exampleCompositeSetting";
        const string settingName1 = "one";
        const string settingName2 = "hello";

        public CompositeSettings()
        {
            this.InitializeComponent();

            roamingSettings = ApplicationData.Current.RoamingSettings;

            DisplayOutput();
        }

        void WriteCompositeSetting_Click(Object sender, RoutedEventArgs e)
        {
            ApplicationDataCompositeValue composite = new ApplicationDataCompositeValue();
            composite[settingName1] = 1; // example value
            composite[settingName2] = "world"; // example value

            roamingSettings.Values[settingName] = composite;

            DisplayOutput();
        }

        void DeleteCompositeSetting_Click(Object sender, RoutedEventArgs e)
        {
            roamingSettings.Values.Remove(settingName);

            DisplayOutput();
        }

        void DisplayOutput()
        {
            ApplicationDataCompositeValue composite = (ApplicationDataCompositeValue)roamingSettings.Values[settingName];

            String output;
            if (composite == null)
            {
                output = "Composite Setting: <empty>";
            }
            else
            {
                output = String.Format("Composite Setting: {{{0} = {1}, {2} = \"{3}\"}}", settingName1, composite[settingName1], settingName2, composite[settingName2]);
            }

            OutputTextBlock.Text = output;
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }
    }
}
