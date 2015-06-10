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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using LiveDash.Util;
using System.Collections.Generic;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace LiveDashApp
{

    class DASHEntry
    {
        private string entryTitle;
        private string sourceUrl;

        public DASHEntry(string title, string url)
        {
            entryTitle = title;
            sourceUrl = url;
        }

        public string Title
        {
            get { return entryTitle; }
        }

        public string Url
        {
            get { return sourceUrl; }
        }
    }
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class LiveDashPlayer : Page
    {
        private MainPage rootPage;
        private LiveDash.LiveDashPlayer player;
        private bool haveSetSource;
        private bool haveSetLiveOffset;
        private TimeSpan liveOffset;


        public LiveDashPlayer()
        {
            this.InitializeComponent();
            scrollView.HorizontalScrollMode = ScrollMode.Enabled;
            scrollView.HorizontalScrollBarVisibility = ScrollBarVisibility.Visible;

            PopulateComboBox();
        }


        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (player != null)
            {
                player.Close();
                player = null;
            }
            haveSetSource = false;
        }

        private async void playButton_Click(object sender, RoutedEventArgs e)
        {
            // Get URI
            if (player != null)
            {
                player.Close();
                player = null;
            }
            Uri uri;
            try
            {
                uri = new Uri(sourceUrl.Text);
            }
            catch
            {
                rootPage.NotifyUser("Invalid URL given", NotifyType.ErrorMessage);
                return;
            }

            haveSetSource = true;
            player = new LiveDash.LiveDashPlayer();

            if (haveSetLiveOffset)
            {
                player.DesiredLiveOffset(liveOffset);
            }

            await player.Initialize(uri, mediaElement);

            // Clear the status block when you press play
            rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage);

        }

        private void setLiveOffsetButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                double liveOffsetDouble = double.Parse(liveOffsetValue.Text);
                TimeSpan offset = TimeSpan.FromSeconds(liveOffsetDouble);
                liveOffset = offset;

                if (liveOffsetDouble < 0)
                {
                    rootPage.NotifyUser("Live offset cannot be negative", NotifyType.ErrorMessage);
                    return;
                }

                if (liveOffsetDouble < 30)
                {
                    rootPage.NotifyUser("Cannot set a live offset less than 30 seconds", NotifyType.ErrorMessage);
                    return;
                }

                if (liveOffsetDouble % 1 != 0)
                {
                    rootPage.NotifyUser("Input must be a whole number (no decimals)", NotifyType.ErrorMessage);
                    return;
                }

                haveSetLiveOffset = true;
                rootPage.NotifyUser("Live offset has been set to " + offset.TotalSeconds + " seconds", NotifyType.StatusMessage);
            }
            catch (Exception)
            {
                rootPage.NotifyUser("Invalid live offset given", NotifyType.ErrorMessage);
            }


        }
        private void gotToLiveButton_Click(object sender, RoutedEventArgs e)
        {
            if (haveSetSource)
            {
                if (haveSetLiveOffset)
                {
                    player.DesiredLiveOffset(liveOffset);
                }
                player.GoToLive();
                rootPage.NotifyUser(String.Empty, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("You must set the source first before going to live", NotifyType.ErrorMessage);
            }
        }
        private void PopulateComboBox()
        {
            IList<DASHEntry> dashEntryList = new List<DASHEntry>();
            dashEntryList.Add(new DASHEntry("DASH-IF Live Stream 1 - Counter", "http://54.201.151.65/livesim/testpic_2s/Manifest.mpd"));
            dashEntryList.Add(new DASHEntry("DASH-IF Live Stream 2 - Counter", "http://eu1.eastmark.net/pdash/testpic_6s/Manifest.mpd"));

            //Add urls to list here

            comboBox1.DataContext = dashEntryList;

        }


        private void ComboBox_SelectionChanged(object sender, RoutedEventArgs e)
        {
            var selected = ((ComboBox)sender).SelectedItem;
            sourceUrl.Text = ((DASHEntry)selected).Url;
        }


    }
}
