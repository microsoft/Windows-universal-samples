//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using SDKTemplate.Common;
using System;
using Windows.UI.Xaml.Controls;
using System.Collections.Generic;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Secondary Tile C#";

        #region Secondary Tile scenario specific variables

        public const string logoSecondaryTileId = "SecondaryTile.Logo";
        public const string dynamicTileId = "SecondaryTile.LiveTile";
        public const string appbarTileId = "SecondaryTile.AppBar";

        #endregion


        List<Scenario> desktopscenarios = new List<Scenario>
        {
            new Scenario() { Title = "Pin Tile",                            ClassType = typeof(SecondaryTiles.PinTile) },
            new Scenario() { Title = "Unpin Tile",                          ClassType = typeof(SecondaryTiles.UnpinTile) },
            new Scenario() { Title = "Enumerate Tiles",                     ClassType = typeof(SecondaryTiles.EnumerateTiles) },
            new Scenario() { Title = "Is Tile Pinned?",                     ClassType = typeof(SecondaryTiles.TilePinned) },
            new Scenario() { Title = "Show Activation Arguments",           ClassType = typeof(SecondaryTiles.LaunchedFromSecondaryTile) },
            new Scenario() { Title = "Secondary Tile Notifications",        ClassType = typeof(SecondaryTiles.SecondaryTileNotification) },
            new Scenario() { Title = "Pin/Unpin Through Appbar",            ClassType = typeof(SecondaryTiles.PinFromAppbar) },
            new Scenario() { Title = "Update Secondary Tile Default Logo",  ClassType = typeof(SecondaryTiles.UpdateAsync) },
            new Scenario() { Title = "Pin Tile Alternate Visual Elements",  ClassType = typeof(SecondaryTiles.PinTileAlternateVisualElements) },
            new Scenario() { Title = "Pin Tile Alternate Visual Elements Async",  ClassType = typeof(SecondaryTiles.PinTileAlternateVisualElementsAsync) }
        };

        List<Scenario> mobilescenarios = new List<Scenario>
        {
            new Scenario() { Title = "Pin Tile",                            ClassType = typeof(SecondaryTiles.PinTile) },
            new Scenario() { Title = "Unpin Tile",                          ClassType = typeof(SecondaryTiles.UnpinTile) },
            new Scenario() { Title = "Enumerate Tiles",                     ClassType = typeof(SecondaryTiles.EnumerateTiles) },
            new Scenario() { Title = "Is Tile Pinned?",                     ClassType = typeof(SecondaryTiles.TilePinned) },
            new Scenario() { Title = "Show Activation Arguments",           ClassType = typeof(SecondaryTiles.LaunchedFromSecondaryTile) },
            new Scenario() { Title = "Secondary Tile Notifications",        ClassType = typeof(SecondaryTiles.SecondaryTileNotification) },
            new Scenario() { Title = "Pin/Unpin Through Appbar",            ClassType = typeof(SecondaryTiles.PinFromAppbar) },
            new Scenario() { Title = "Update Secondary Tile Default Logo",  ClassType = typeof(SecondaryTiles.UpdateAsync) },
            new Scenario() { Title = "Pin Tile and Update on Suspend",      ClassType = typeof(SecondaryTiles.PinTileAndUpdateOnSuspend) }
        };


        #region Secondary Tile specific methods

        // Gets the rectangle of the element
        public static Rect GetElementRect(FrameworkElement element)
        {
            GeneralTransform buttonTransform = element.TransformToVisual(null);
            Point point = buttonTransform.TransformPoint(new Point());
            return new Rect(point, new Size(element.ActualWidth, element.ActualHeight));
        }

        partial void GetScenarioIdForLaunch(string launchParam, ref int index)
        {
            index = -1;
            // Populate the ListBox with the list of scenarios as defined in Constants.cs.
            if (Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
            {
                foreach (Scenario s in mobilescenarios)
                {
                    index++;
                    if (s.ClassType == typeof(SecondaryTiles.LaunchedFromSecondaryTile))
                    {
                        break;
                    }
                }
            }
            else
            {
                foreach (Scenario s in desktopscenarios)
                {
                    index++;
                    if (s.ClassType == typeof(SecondaryTiles.LaunchedFromSecondaryTile))
                    {
                        break;
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

        public override string ToString()
        {
            return Title;
        }
    }
}
