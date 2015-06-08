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

using System.Collections.Generic;
using Windows.Foundation;
using Windows.Storage.Search;
using Windows.UI.Text;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using System;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario1()
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
            FindQueryText.Text = "continent OR can OR rain";
            ContentText.Text = "Mount Rainier is on the North American Continent.";
        }

        private void Find_Click(object sender, RoutedEventArgs e)
        {
            ContentTextOutput.Text = "";
            var mySemanticTextQuery = new Windows.Data.Text.SemanticTextQuery(FindQueryText.Text);
            IReadOnlyList<Windows.Data.Text.TextSegment> ranges = mySemanticTextQuery.Find(ContentText.Text);
            rootPage.HighlightRanges(ContentTextOutput, ContentText.Text, ranges);
        }
    }
}
