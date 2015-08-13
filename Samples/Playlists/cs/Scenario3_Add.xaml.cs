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
    public sealed partial class Scenario3_Add : Page
    {
        private MainPage rootPage = MainPage.Current;
        private Playlist playlist = null;

        public Scenario3_Add()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Loads a playlist picked by the user in the FilePicker
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void PickPlaylistButton_Click(object sender, RoutedEventArgs e)
        {
            PickAudioButton.IsEnabled = false;

            this.playlist = await rootPage.PickPlaylistAsync();

            if (this.playlist != null)
            {
                rootPage.NotifyUser("Playlist loaded.", NotifyType.StatusMessage);
            }

            PickAudioButton.IsEnabled = (this.playlist != null);

        }

        /// <summary>
        /// Adds a file to the end of the playlist loaded in PickPlaylistButton_Click
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void PickAudioButton_Click(object sender, RoutedEventArgs e)
        {
            FileOpenPicker picker = MainPage.CreateFilePicker(MainPage.audioExtensions);
            IReadOnlyList<StorageFile> files = await picker.PickMultipleFilesAsync();

            if (files.Count > 0)
            {
                foreach (StorageFile file in files)
                {
                    this.playlist.Files.Add(file);
                }

                if (await rootPage.TrySavePlaylistAsync(playlist))
                {
                    rootPage.NotifyUser(files.Count + " files added to playlist.", NotifyType.StatusMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("No files picked.", NotifyType.ErrorMessage);
            }
        }
    }
}
