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

using BackgroundMediaPlayback.ViewModels;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
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

namespace BackgroundMediaPlayback
{
    /// <summary>
    /// A custom control for playlists that wraps a ListView.
    /// </summary>
    public sealed partial class PlaylistView : UserControl, INotifyPropertyChanged
    {
        MediaListViewModel mediaList;

        public event PropertyChangedEventHandler PropertyChanged;

        public PlaylistView()
        {
            this.InitializeComponent();
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
        public MediaListViewModel MediaList
        {
            get
            {
                return mediaList;
            }
            set
            {
                if (mediaList != value)
                {
                    mediaList = value;

                    // Setting ItemsSource = null on page unload throws, so avoid that
                    if (value != null)
                        listView.ItemsSource = mediaList;

                    RaisePropertyChanged("MediaList");
                }
            }
        }

        void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
