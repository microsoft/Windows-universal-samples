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
using System.Collections.Generic;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Title bar";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Custom colors", ClassType=typeof(Scenario1_Colors)},
            new Scenario() { Title="Custom drawing", ClassType=typeof(Scenario2_Extend)},
        };

        #region Custom title bar
        CustomTitleBar customTitleBar = null;

        public void AddCustomTitleBar()
        {
            if (customTitleBar == null)
            {
                customTitleBar = new CustomTitleBar();
                customTitleBar.EnableControlsInTitleBar(areControlsInTitleBar);

                // Make the main page's content a child of the title bar,
                // and make the title bar the new page content.
                UIElement mainContent = this.Content;
                this.Content = null;
                customTitleBar.SetPageContent(mainContent);
                this.Content = customTitleBar;
            }
        }

        public void RemoveCustomTitleBar()
        {
            if (customTitleBar != null)
            {
                // Take the title bar's page content and make it
                // the window content.
                this.Content = customTitleBar.SetPageContent(null);
                customTitleBar = null;
            }
        }

        bool areControlsInTitleBar = false;

        public bool AreControlsInTitleBar
        {
            get
            {
                return areControlsInTitleBar;
            }
            set
            {
                areControlsInTitleBar = value;
                if (customTitleBar != null)
                {
                    customTitleBar.EnableControlsInTitleBar(value);
                }
            }
        }

        #endregion
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
