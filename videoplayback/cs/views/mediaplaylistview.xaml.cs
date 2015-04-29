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
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace VideoPlayback
{
    /// <summary>
    /// A custom control for playlists that wraps a ListView.
    /// </summary>
    public sealed partial class MediaPlaylistView : UserControl
    {
        ObservableCollection<MediaModel> media = new ObservableCollection<MediaModel>();

        public MediaPlaylistView()
        {
            this.InitializeComponent();

            // Bind the list view to the songs collection
            listView.ItemsSource = Media;
        }

        /// <summary>
        /// The selected index of the playlist
        /// </summary>
        public int SelectedIndex
        {
            get
            {
                return listView.SelectedIndex;
            }
            set
            {
                listView.SelectedIndex = value;
            }
        }

        /// <summary>
        /// Raised when an item is clicked
        /// </summary>
        public event ItemClickEventHandler ItemClick
        {
            add { listView.ItemClick += value; }
            remove { listView.ItemClick -= value; }
        }

        /// <summary>
        /// Raised when the selection changes
        /// </summary>
        public event SelectionChangedEventHandler SelectionChanged
        {
            add { listView.SelectionChanged += value; }
            remove { listView.SelectionChanged -= value; }
        }
        
        /// <summary>
        /// A collection of songs in the list to be displayed to the user
        /// </summary>
        public ObservableCollection<MediaModel> Media
        {
            get
            {
                return media;
            }
        }

        public MediaModel GetSongById(Uri id)
        {
            return media.Single(s => s.MediaUri == id);
        }

        public int GetSongIndexById(Uri id)
        {
            return media.ToList().FindIndex(s => s.MediaUri == id);
        }
    }
}
