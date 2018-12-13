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
using System.Threading.Tasks;
using Windows.Devices.PointOfService;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Line Display C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Selecting a Line Display", ClassType=typeof(Scenario1_SelectDisplay)},
            new Scenario() { Title="Displaying text", ClassType=typeof(Scenario2_DisplayText)},
            new Scenario() { Title="Using windows to control layout", ClassType=typeof(Scenario3_UsingWindows)},
            new Scenario() { Title="Updating line display attributes", ClassType=typeof(Scenario4_UpdatingLineDisplayAttributes)},
            new Scenario() { Title="Defining custom glyphs", ClassType=typeof(Scenario5_DefiningCustomGlyphs)},
            new Scenario() { Title="Modifying the display cursor", ClassType=typeof(Scenario6_ManipulatingCursorAttributes)},
            new Scenario() { Title="Scrolling content using marquee", ClassType=typeof(Scenario7_ScrollingContentUsingMarquee)},
        };

        // The device ID of the selected line display.
        public string LineDisplayId = null;

        public async Task<ClaimedLineDisplay> ClaimScenarioLineDisplayAsync()
        {
            ClaimedLineDisplay lineDisplay = null;
            if (String.IsNullOrEmpty(LineDisplayId))
            {
                NotifyUser("You must use scenario 1 to select a line display", NotifyType.ErrorMessage);
            }
            else
            {
                lineDisplay = await ClaimedLineDisplay.FromIdAsync(LineDisplayId);
                if (lineDisplay == null)
                {
                    NotifyUser("Unable to claim selected LineDisplay from id.", NotifyType.ErrorMessage);
                }
            }
            return lineDisplay;
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    static class Helpers
    {
        public static double ParseUIntWithFallback(TextBox textBox, double fallback)
        {
            uint value;
            if (uint.TryParse(textBox.Text, out value))
            {
                return value;
            }
            return fallback;
        }

        public static T GetSelectedItemTag<T>(Selector selector)
        {
            return (T)((FrameworkElement)selector.SelectedItem).Tag;
        }
    }
}
