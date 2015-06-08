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

using AdventureWorks.Common;
using AdventureWorks.ViewModel;
using System;
using System.Collections.Generic;
using System.Linq;
using Windows.ApplicationModel;
using Windows.Media.SpeechRecognition;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace AdventureWorks.View
{
    /// <summary>
    /// Code Behind for a list of trips, including an Add button. Associated with the 
    /// TripListViewModel for most behaviors and properties.
    /// </summary>
    public sealed partial class TripListView : Page
    {
        private NavigationHelper navigationHelper;

        private TripListViewModel defaultViewModel;

        public TripListViewModel DefaultViewModel
        {
            get { return defaultViewModel; }
        }

        /// <summary>
        /// NavigationHelper is used on each page to aid in navigation and 
        /// process lifetime management
        /// </summary>
        public NavigationHelper NavigationHelper
        {
            get { return this.navigationHelper; }
        }


        public TripListView()
        {
            this.InitializeComponent();
            this.navigationHelper = new NavigationHelper(this);
            this.navigationHelper.LoadState += navigationHelper_LoadState;
            this.navigationHelper.SaveState += navigationHelper_SaveState;
        }

        /// <summary>
        /// If we've lost focus and returned to this app, reload the store, as the on-disk content might have
        /// been changed by the background task.
        /// </summary>
        /// <param name="sender">Ignored</param>
        /// <param name="args">Ignored</param>
        private async void CoreWindow_VisibilityChanged(Windows.UI.Core.CoreWindow sender, Windows.UI.Core.VisibilityChangedEventArgs args)
        {
            if (args.Visible == true)
            {
                await DefaultViewModel.LoadTrips();
            }
        }


        /// <summary>
        /// Sets up the view model.
        /// </summary>
        /// <param name="sender">
        /// The source of the event; typically <see cref="Common.NavigationHelper"/>
        /// </param>
        /// <param name="e">Event data that provides both the navigation parameter passed to
        /// <see cref="Frame.Navigate(Type, Object)"/> when this page was initially requested and
        /// a dictionary of state preserved by this page during an earlier
        /// session. The state will be null the first time a page is visited.</param>
        private void navigationHelper_LoadState(object sender, LoadStateEventArgs e)
        {
            defaultViewModel = (TripListViewModel)this.DataContext;

            DefaultViewModel.SelectedTrip = null;

            Window.Current.CoreWindow.VisibilityChanged += CoreWindow_VisibilityChanged;
        }

        /// <summary>
        /// Preserves state associated with this page in case the application is suspended or the
        /// page is discarded from the navigation cache.  Values must conform to the serialization
        /// requirements of <see cref="Common.SuspensionManager.SessionState"/>.
        /// </summary>
        /// <param name="sender">The source of the event; typically <see cref="Common.NavigationHelper"/></param>
        /// <param name="e">Event data that provides an empty dictionary to be populated with
        /// serializable state.</param>
        private void navigationHelper_SaveState(object sender, SaveStateEventArgs e)
        {
        }

        #region NavigationHelper registration

        /// The methods provided in this section are simply used to allow
        /// NavigationHelper to respond to the page's navigation methods.
        /// 
        /// Page specific logic should be placed in event handlers for the  
        /// <see cref="Common.NavigationHelper.LoadState"/>
        /// and <see cref="Common.NavigationHelper.SaveState"/>.
        /// The navigation parameter is available in the LoadState method 
        /// in addition to page state preserved during an earlier session.

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            navigationHelper.OnNavigatedTo(e);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            navigationHelper.OnNavigatedFrom(e);
        }

        #endregion

        /// <summary>
        /// Since there's an imperfect click implementation on listboxes, this provides a quick
        /// way to handle the SelectionChanged event, in order to open the selected trip's 
        /// details.
        /// </summary>
        /// <param name="sender">Ignored</param>
        /// <param name="e">Ignored</param>
        private void tripListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            this.DefaultViewModel.SelectionChanged();
        }

        /// <summary>
        /// Handle footer link clicks.
        /// </summary>
        /// <param name="sender">The target uri</param>
        /// <param name="e">Ignored</param>
        async void Footer_Click(object sender, RoutedEventArgs e)
        {
            await Windows.System.Launcher.LaunchUriAsync(new Uri(((HyperlinkButton)sender).Tag.ToString()));
        }
    }
}
