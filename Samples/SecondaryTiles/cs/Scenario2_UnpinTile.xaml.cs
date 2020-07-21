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
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario2_UnpinTile : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario2_UnpinTile()
        {
            this.InitializeComponent();
        }

        private async void UnpinSecondaryTile_Click(object sender, RoutedEventArgs e)
        {
            if (SecondaryTile.Exists(MainPage.logoSecondaryTileId))
            {
                // First prepare the tile to be unpinned
                SecondaryTile secondaryTile = new SecondaryTile(MainPage.logoSecondaryTileId);
                // Now make the delete request.
                bool isUnpinned = await secondaryTile.RequestDeleteForSelectionAsync(MainPage.GetElementRect(sender), Windows.UI.Popups.Placement.Below);
                if (isUnpinned)
                {
                    rootPage.NotifyUser("Secondary tile successfully unpinned.", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Secondary tile not unpinned.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser(MainPage.logoSecondaryTileId + " is not currently pinned.", NotifyType.ErrorMessage);
            }
        }
    }
}
