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
using AdventureWorks.Model;
using AdventureWorks.ViewModel;
using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Basic Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234237

namespace AdventureWorks.View
{
    /// <summary>
    /// Code Behind for a list of trips, including a Save and Delete button. Associated with the 
    /// TripViewModel for most behaviors and properties.
    /// </summary>
    public sealed partial class TripDetails : Page
    {
        private NavigationHelper navigationHelper;
        private TripViewModel defaultViewModel;

        /// <summary>
        /// The ViewModel that provides behaviors and properties associated with displaying a Trip's
        /// details.
        /// </summary>
        public TripViewModel DefaultViewModel
        {
            get { return this.defaultViewModel; }
        }

        /// <summary>
        /// NavigationHelper is used on each page to aid in navigation and 
        /// process lifetime management
        /// </summary>
        public NavigationHelper NavigationHelper
        {
            get { return this.navigationHelper; }
        }


        public TripDetails()
        {
            this.InitializeComponent();
            this.navigationHelper = new NavigationHelper(this);
            this.navigationHelper.LoadState += navigationHelper_LoadState;
            this.navigationHelper.SaveState += navigationHelper_SaveState;
        }

        /// <summary>
        /// Populates the page with content passed during navigation. Any saved state is also
        /// provided when recreating a page from a prior session.
        /// Resets the state of the view model by passing in the parameters provided by the
        /// caller.
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
            defaultViewModel = (TripViewModel)this.DataContext;
            if (e.NavigationParameter is Trip)
            {
                // Activated via selecting a trip in the main page's list of trips.
                DefaultViewModel.ShowTrip((Trip)e.NavigationParameter);
            }
            else if (e.NavigationParameter is TripVoiceCommand)
            {
                // look up destination, set trip.
                TripVoiceCommand voiceCommand = (TripVoiceCommand)e.NavigationParameter;
                DefaultViewModel.LoadTripFromStore(voiceCommand.destination);

                // artificially populate the page backstack so we have something to
                // go back to to get to the main page.
                PageStackEntry backEntry = new PageStackEntry(typeof(View.TripListView), null, null);
                this.Frame.BackStack.Add(backEntry);
            }
            else if(e.NavigationParameter is string)
            {
                // We've been URI Activated, possibly by a user clicking on a tile in a Cortana session,
                // we should see an argument like destination=<Location>. 
                // This should handle finding all of the destinations that match, but currently it only
                // finds the first one.
                string arguments = e.NavigationParameter as string;
                if(arguments != null)
                {
                    string[] args = arguments.Split('=');
                    if (args.Length == 2 && args[0].ToLowerInvariant() == "destination")
                    {
                        DefaultViewModel.LoadTripFromStore(args[1]);

                        // artificially populate the page backstack so we have something to
                        // go back to to get to the main page.
                        PageStackEntry backEntry = new PageStackEntry(typeof(View.TripListView), null, null);
                        this.Frame.BackStack.Add(backEntry);
                    }
                }
            }
            else
            {
                DefaultViewModel.NewTrip();
            }
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
