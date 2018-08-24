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
using Windows.Foundation.Metadata;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Maps;

namespace SDKTemplate
{
    /// <summary>
    /// Shows how to customize the look and feel of the map
    /// </summary>
    public sealed partial class Scenario9 : Page
    {
        public Scenario9()
        {
            this.InitializeComponent();
        }

        private void MyMap_Loaded(object sender, RoutedEventArgs e)
        {
            myMap.Center = MainPage.SeattleGeopoint;
            myMap.ZoomLevel = 12;
            SetMapStyleSheet();
        }

        private void SetDefaultStyleCombobox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // Protect against events that are raised before we are fully initialized.
            if (myMap != null)
            {
                SetMapStyleSheet();
            }
        }

        private void SetMapStyleSheet()
        {
            switch (setDefaultStyleCombobox.SelectedIndex)
            {
                case 0:
                    myMap.StyleSheet = MapStyleSheet.Aerial();
                    break;
                case 1:
                    myMap.StyleSheet = MapStyleSheet.AerialWithOverlay();
                    break;
                case 2:
                    myMap.StyleSheet = MapStyleSheet.RoadDark();
                    break;
                case 3:
                    myMap.StyleSheet = MapStyleSheet.RoadLight();
                    break;
                case 4:
                    myMap.StyleSheet = MapStyleSheet.RoadHighContrastDark();
                    break;
                case 5:
                    myMap.StyleSheet = MapStyleSheet.RoadHighContrastLight();
                    break;
                case 6:
                    myMap.StyleSheet = MapStyleSheet.ParseFromJson(@"
                        {
                            ""version"": ""1.0"",
                            ""settings"": {
                                ""landColor"": ""#FFFFFF"",
                                ""spaceColor"": ""#000000""
                            },
                            ""elements"": {
                                ""mapElement"": {
                                    ""labelColor"": ""#000000"",
                                    ""labelOutlineColor"": ""#FFFFFF""
                                },
                                ""water"": {
                                    ""fillColor"": ""#DDDDDD""
                                },
                                ""area"": {
                                    ""fillColor"": ""#EEEEEE""
                                },
                                ""political"": {
                                    ""borderStrokeColor"": ""#CCCCCC"",
                                    ""borderOutlineColor"": ""#00000000""
                                }
                            }
                        }
                    ");
                    break;
            }
        }
    }
}
