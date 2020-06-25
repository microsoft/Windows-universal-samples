//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario4_TilePinned : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario4_TilePinned()
        {
            this.InitializeComponent();
        }

        private void TileExists_Click(object sender, RoutedEventArgs e)
        {
            if (SecondaryTile.Exists(MainPage.logoSecondaryTileId))
            {
                rootPage.NotifyUser(MainPage.logoSecondaryTileId + " is pinned.", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser(MainPage.logoSecondaryTileId + " is not currently pinned.", NotifyType.ErrorMessage);
            }
        }
    }
}
