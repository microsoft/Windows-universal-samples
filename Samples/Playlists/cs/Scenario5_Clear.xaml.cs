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
using Windows.Media.Playlists;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario5_Clear : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario5_Clear()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Clears the playlist picked by the user in the FilePicker
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void PickPlaylistButton_Click(object sender, RoutedEventArgs e)
        {
            Playlist playlist = await rootPage.PickPlaylistAsync();

            if (playlist != null)
            {
                playlist.Files.Clear();

                if (await rootPage.TrySavePlaylistAsync(playlist))
                {
                    this.rootPage.NotifyUser("Playlist cleared.", NotifyType.StatusMessage);
                }
            }
        }
    }
}
