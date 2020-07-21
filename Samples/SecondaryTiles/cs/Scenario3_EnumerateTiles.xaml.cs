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
using System.Collections.Generic;
using System.Text;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario3_EnumerateTiles : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario3_EnumerateTiles()
        {
            this.InitializeComponent();
        }

        private async void EnumerateSecondaryTiles_Click(object sender, RoutedEventArgs e)
        {
            // Get secondary tile ids for this package
            IReadOnlyList<SecondaryTile> tilelist = await SecondaryTile.FindAllAsync();            
            if (tilelist.Count > 0)
            {
                StringBuilder outputText = new StringBuilder();
                foreach (var tile in tilelist)
                {
                    outputText.AppendLine($"Tile \"{tile.TileId}\", Display name = \"{tile.DisplayName}\"");
                }
                rootPage.NotifyUser(outputText.ToString(), NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("No secondary tiles are available for this appId.", NotifyType.ErrorMessage);
            }
        }
    }
}
