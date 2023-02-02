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
using System;
using Windows.Storage;

namespace SDKTemplate
{
    public sealed partial class Scenario4_CompositeSettings : Page
    {
        ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;

        const string settingName = "exampleCompositeSetting";
        const string settingName1 = "one";
        const string settingName2 = "hello";

        public Scenario4_CompositeSettings()
        {
            InitializeComponent();
        }

        void WriteCompositeSetting_Click(Object sender, RoutedEventArgs e)
        {
            // Composite settings are used to group multiple settings.
            // Note that Composite settings may not be nested.
            ApplicationDataCompositeValue composite = new ApplicationDataCompositeValue();
            composite[settingName1] = 1; // example value
            composite[settingName2] = "world"; // example value

            localSettings.Values[settingName] = composite;

            DisplayOutput();
        }

        void DeleteCompositeSetting_Click(Object sender, RoutedEventArgs e)
        {
            localSettings.Values.Remove(settingName);

            DisplayOutput();
        }

        void DisplayOutput()
        {
            ApplicationDataCompositeValue composite = (ApplicationDataCompositeValue)localSettings.Values[settingName];

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

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            DisplayOutput();
        }
    }
}
