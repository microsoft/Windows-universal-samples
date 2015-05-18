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
using AdventureWorks.View;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace AdventureWorks.ViewModel
{
    /// <summary>
    /// View Model controlling the behavior of a List view of trips 
    /// </summary>
    public class TripListViewModel : ViewModelBase
    {
        private ICommand addTripCommand;
        private ObservableCollection<Trip> trips;
        private Trip selectedTrip;
        private TripStore store;

        /// <summary>
        /// Construct the trip view, passing in the persistent trip store. Sets up
        /// a command to handle invoking the Add button.
        /// </summary>
        /// <param name="store"></param>
        public TripListViewModel(TripStore store)
        {
            this.store = store;
            Trips = store.Trips;

            addTripCommand = new RelayCommand(new Action(AddTrip));
        }

        /// <summary>
        /// The list of trips to display on the UI.
        /// </summary>
        public ObservableCollection<Trip> Trips
        {
            get
            {
                return trips;
            }
            private set
            {
                trips = value;
                NotifyPropertyChanged("Trips");
            }
        }

        /// <summary>
        /// A two-way binding that keeps reference to the currently selected trip on
        /// the UI.
        /// </summary>
        public Trip SelectedTrip
        {
            get
            {
                return selectedTrip;
            }
            set
            {
                selectedTrip = value;
                NotifyPropertyChanged("SelectedTrip");
            }
        }

        /// <summary>
        /// The command to invoke when the Add button is pressed.
        /// </summary>
        public ICommand AddTripCommand
        {
            get
            {
                return addTripCommand;
            }
        }

        /// <summary>
        /// Reload the trip store from data files.
        /// </summary>
        internal async Task LoadTrips()
        {
            await store.LoadTrips();
        }


        /// <summary>
        /// The implementation of the command to execute when the Add button is pressed.
        /// </summary>
        private void AddTrip()
        {
            App.NavigationService.Navigate<TripDetails>();
        }

        /// <summary>
        /// Handles a user selecting a trip on the UI by navigating to the TripDetails view
        /// for that trip.
        /// </summary>
        internal void SelectionChanged()
        {
            if (SelectedTrip != null)
            {
                App.NavigationService.Navigate<TripDetails>(SelectedTrip);
            }
        }
    }
}
