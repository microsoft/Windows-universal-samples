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
    public sealed partial class Scenario10 : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario10()
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
        /// This is the click handler for the 'Scenario10Button_Show' button.  You would replace this with your own handler
        /// if you have a button or buttons on this page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Scenario10Button_Show_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            if (b != null)
            {
                // use a cloned context for this scenario so that qualifier values set
                // in this scenario don't impact behaviour in other scenarios that
                // use a default context for the view (crossover effects between
                // the scenarios will not be expected)
                var context = ResourceContext.GetForCurrentView().Clone();

                var selectedLanguage = Scenario10ComboBox_Language.SelectedValue;
                var selectedScale = Scenario10ComboBox_Scale.SelectedValue;
                var selectedContrast = Scenario10ComboBox_Contrast.SelectedValue;
                var selectedHomeRegion = Scenario10ComboBox_HomeRegion.SelectedValue;

                if (selectedLanguage != null)
                {
                    context.QualifierValues["language"] = selectedLanguage.ToString();
                }
                if (selectedScale != null)
                {
                    context.QualifierValues["scale"] = selectedScale.ToString();
                }
                if (selectedContrast != null)
                {
                    context.QualifierValues["contrast"] = selectedContrast.ToString();
                }
                if (selectedHomeRegion != null)
                {
                    context.QualifierValues["homeregion"] = selectedHomeRegion.ToString();
                }

                Scenario10_SearchMultipleResourceIds(context, new string[] { "LanguageOnly", "ScaleOnly", "ContrastOnly", "HomeRegionOnly", "MultiDimensional" });
            }
        }

        void Scenario10_SearchMultipleResourceIds(ResourceContext context, string[] resourceIds)
        {
            this.Scenario10TextBlock.Text = "";
            var dimensionMap = ResourceManager.Current.MainResourceMap.GetSubtree("dimensions");

            foreach (var id in resourceIds)
            {
                NamedResource namedResource;
                if (dimensionMap.TryGetValue(id, out namedResource))
                {
                    var resourceCandidates = namedResource.ResolveAll(context);
                    Scenario10_ShowCandidates(id, resourceCandidates);
                }
            }
        }

        void Scenario10_ShowCandidates(string resourceId, IReadOnlyList<ResourceCandidate> resourceCandidates)
        {
            // print 'resourceId', 'found value', 'qualifer info', 'matching condition'
            string outText = "resourceId: dimensions\\" + resourceId + "\r\n";
            int i = 0;
            foreach (var candidate in resourceCandidates)
            {
                var value = candidate.ValueAsString;
                outText += "    Candidate " + i.ToString() + ":" + value + "\r\n";
                int j = 0;
                foreach (var qualifier in candidate.Qualifiers)
                {
                    var qualifierName = qualifier.QualifierName;
                    var qualifierValue = qualifier.QualifierValue;
                    outText += "        Qualifer: " + qualifierName + ": " + qualifierValue + "\r\n";
                    outText += "        Matching: IsMatch (" + qualifier.IsMatch.ToString() + ")  " + "IsDefault (" + qualifier.IsDefault.ToString() + ")" + "\r\n";
                    j++;
                }
                i++;
            }

            this.Scenario10TextBlock.Text += outText + "\r\n";

        }

    }
}

