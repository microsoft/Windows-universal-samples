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
using Windows.Storage.FileProperties;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2_Display : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario2_Display()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Displays the playlist picked by the user in the FilePicker
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void PickPlaylistButton_Click(object sender, RoutedEventArgs e)
        {
            this.rootPage.NotifyUser("", NotifyType.StatusMessage);
            Playlist playlist = await this.rootPage.PickPlaylistAsync();

            if (playlist != null)
            {
                string result = "Songs in playlist: " + playlist.Files.Count.ToString() + "\n";

                foreach (StorageFile file in playlist.Files)
                {
                    MusicProperties properties = await file.Properties.GetMusicPropertiesAsync();
                    result += "\n";
                    result += "File: " + file.Path + "\n";
                    result += "Title: " + properties.Title + "\n";
                    result += "Album: " + properties.Album + "\n";
                    result += "Artist: " + properties.Artist + "\n";
                }

                this.OutputStatus.Text = result;
            }
        }
    }
}
