//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using Windows.UI.Xaml.Controls;
using System.Collections.Generic;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;
using Windows.ApplicationModel.Activation;

namespace SDKTemplate
{
    public partial class App
    {
        partial void LaunchCompleted(LaunchActivatedEventArgs e)
        {
            MainPage.Current.LaunchParam = e.Arguments;
        }
    }

    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Secondary Tile C#";

        #region Secondary Tile scenario specific variables

        public const string logoSecondaryTileId = "SecondaryTile.Logo";
        public const string dynamicTileId = "SecondaryTile.LiveTile";
        public const string appbarTileId = "SecondaryTile.AppBar";

        #endregion

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Pin Tile",                            ClassType = typeof(SDKTemplate.Scenario1_PinTile) },
            new Scenario() { Title = "Unpin Tile",                          ClassType = typeof(SDKTemplate.Scenario2_UnpinTile) },
            new Scenario() { Title = "Enumerate Tiles",                     ClassType = typeof(SDKTemplate.Scenario3_EnumerateTiles) },
            new Scenario() { Title = "Is Tile Pinned?",                     ClassType = typeof(SDKTemplate.Scenario4_TilePinned) },
            new Scenario() { Title = "Show Activation Arguments",           ClassType = typeof(SDKTemplate.Scenario5_LaunchedFromSecondaryTile) },
            new Scenario() { Title = "Secondary Tile Notifications",        ClassType = typeof(SDKTemplate.Scenario6_SecondaryTileNotification) },
            new Scenario() { Title = "Pin/Unpin Through Appbar",            ClassType = typeof(SDKTemplate.Scenario7_PinFromAppbar) },
            new Scenario() { Title = "Update Secondary Tile Default Logo",  ClassType = typeof(SDKTemplate.Scenario8_UpdateAsync) },
        };

        #region Secondary Tile specific methods

        // Gets the rectangle of the element
        public static Rect GetElementRect(object e)
        {
            var element = (FrameworkElement)e;
            GeneralTransform buttonTransform = element.TransformToVisual(null);
            return buttonTransform.TransformBounds(new Rect(0, 0, element.ActualWidth, element.ActualHeight));
        }

        string launchParam;
        public string LaunchParam
        {
            get => launchParam;
            set
            {
                launchParam = value;
                if (!string.IsNullOrEmpty(launchParam))
                {
                    // Force a navigation to the secondary tile launch page.
                    for (int index = 0; index < scenarios.Count; index++)
                    {
                        if (scenarios[index].ClassType == typeof(SDKTemplate.Scenario5_LaunchedFromSecondaryTile))
                        {
                            if (ScenarioControl.SelectedIndex == index)
                            {
                                ScenarioControl.SelectedIndex = -1;
                            }
                            ScenarioControl.SelectedIndex = index;
                        }
                    }
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
