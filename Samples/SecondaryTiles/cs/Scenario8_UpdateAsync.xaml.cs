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
    public sealed partial class Scenario8_UpdateAsync : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario8_UpdateAsync()
        {
            this.InitializeComponent();
        }

        private async void UpdateDefaultLogo_Click(object sender, RoutedEventArgs e)
        {
            if (SecondaryTile.Exists(MainPage.logoSecondaryTileId))
            {
                // Add the properties we want to update (logo in this example)
                SecondaryTile secondaryTile = new SecondaryTile(MainPage.logoSecondaryTileId);
                secondaryTile.VisualElements.Square150x150Logo = new Uri("ms-appx:///Assets/squareTileLogoUpdate-sdk.png");
                bool isUpdated = await secondaryTile.UpdateAsync();

                if (isUpdated)
                {
                    rootPage.NotifyUser("Secondary tile logo updated.", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Secondary tile logo not updated.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Please pin a secondary tile using scenario 1 before updating the logo.", NotifyType.ErrorMessage);
            }
        }
    }
}
