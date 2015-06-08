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
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using Windows.Media.SpeechRecognition;
using Windows.ApplicationModel.VoiceCommands;

namespace AdventureWorks.ViewModel
{
    /// <summary>
    /// View Model associated with TripDetails.xaml. Provides access to an individual Trip,
    /// and Commands for saving new, updating existing, and deleting trips.
    /// Is able to both create brand new trips, and edit existing trips, hiding buttons that
    /// should not be accessible in some cases.
    /// </summary>
    public class TripViewModel : ViewModelBase
    {
        private ICommand saveTripCommand;
        private ICommand deleteTripCommand;
        private Trip trip;
        private TripStore store;

        private bool showDestinationValidation = false;
        private string destinationValidationError;

        private bool showDelete = false;

        /// <summary>
        /// The Trip this view model represents.
        /// </summary>
        public Trip Trip
        {
            get
            {
                return trip;
            }
            set
            {
                trip = value;
                NotifyPropertyChanged("Trip");
            }
        }

        /// <summary>
        /// We require that a destination be set to a non-empty string. If the user
        /// attempts to save without one, this will be set to an explanatory validation
        /// prompt to be rendered in the UI.
        /// </summary>
        public String DestinationValidationError
        {
            get
            {
                return destinationValidationError;
            }

            private set
            {
                destinationValidationError = value;
                NotifyPropertyChanged("DestinationValidationError");
            }
        }

        /// <summary>
        /// Controls whether the TextBlock that contains the destination validation string
        /// is visible. Combined with the VisibilityConverter, can show or collapse elements.
        /// </summary>
        public bool ShowDestinationValidation
        {
            get
            {
                return showDestinationValidation;
            }
            private set
            {
                showDestinationValidation = value;
                NotifyPropertyChanged("ShowDestinationValidation");
            }
        }

        /// <summary>
        /// Controls whether the Button that deletes trips is shown. If creating a new trip,
        /// this is false. Otherwise, true.
        /// </summary>
        public bool ShowDelete
        {
            get
            {
                return showDelete;
            }
            private set
            {
                showDelete = value;
                NotifyPropertyChanged("ShowDelete");
            }
        }

        /// <summary>
        /// Bound to the save button, provides a command to invoke when pressed.
        /// </summary>
        public ICommand SaveTripCommand
        {
            get
            {
                return saveTripCommand;
            }
        }

        /// <summary>
        /// Load a trip fomr the store, and set up the view per a normal ShowTrip command.
        /// </summary>
        /// <param name="destination"></param>
        internal void LoadTripFromStore(string destination)
        {
            Trip t = store.Trips.Where(p => p.Destination == destination).FirstOrDefault();
            if (t != null)
            {
                this.ShowTrip(t);
            }
            else
            {
                // Redirect back to the main page, and pass along an error message to show
                App.NavigationService.Navigate<View.TripListView>(
                    string.Format(
                        "Sorry, couldn't find a trip with Destination {0}",
                        destination));
            }
        }


        /// <summary>
        /// Bound to the Delete button, provides a command to invoke when pressed.
        /// </summary>
        public ICommand DeleteTripCommand
        {
            get
            {
                return deleteTripCommand;
            }
        }

        /// <summary>
        /// Construct Trip ViewModel, providing the store to persist trips. 
        /// Creates the RelayCommands to be bound to various buttons in the UI.
        /// </summary>
        /// <param name="store">The persistent store</param>
        public TripViewModel(TripStore store)
        {
            trip = new Trip();
            saveTripCommand = new RelayCommand(new Action(SaveTrip));
            deleteTripCommand = new RelayCommand(new Action(DeleteTrip));
            this.store = store;
        }

        /// <summary>
        /// removes a trip from the store, and returns to the trip list.
        /// </summary>
        private async void DeleteTrip()
        {
            await store.DeleteTrip(Trip);
            await UpdateDestinationPhraseList();
            App.NavigationService.GoBack();
        }

        /// <summary>
        /// Whenever a trip is modified, we trigger an update of the voice command Phrase list. This allows
        /// voice commands such as "Adventure Works Show trip to {destination} to be up to date with saved
        /// Trips.
        /// </summary>
        public async Task UpdateDestinationPhraseList()
        {
            try
            {
                // Update the destination phrase list, so that Cortana voice commands can use destinations added by users.
                // When saving a trip, the UI navigates automatically back to this page, so the phrase list will be
                // updated automatically.
                VoiceCommandDefinition commandDefinitionsEnUs;

                if (VoiceCommandDefinitionManager.InstalledCommandDefinitions.TryGetValue("AdventureWorksCommandSet_en-us", out commandDefinitionsEnUs))
                {
                    List<string> destinations = new List<string>();
                    foreach (Model.Trip t in store.Trips)
                    {
                        destinations.Add(t.Destination);
                    }

                    await commandDefinitionsEnUs.SetPhraseListAsync("destination", destinations);
                }
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine("Updating Phrase list for VCDs: " + ex.ToString());
            }
        }


        /// <summary>
        /// Performs validation on the destination to ensure it's not empty, then 
        /// saves a trip to the store. If the destination isn't valid, shows a validation
        /// error.
        /// </summary>
        private async void SaveTrip()
        {
            ShowDestinationValidation = false;
            bool valid = true;

            if (String.IsNullOrEmpty(Trip.Destination))
            {
                valid = false;
                ShowDestinationValidation = true;
                DestinationValidationError = "Destination cannot be blank";
            }
            else
            {
                Trip.Destination = Trip.Destination.Trim();
            }

            if (valid)
            {
                await store.SaveTrip(this.Trip);
                await UpdateDestinationPhraseList();
                App.NavigationService.GoBack();
            }
        }

        /// <summary>
        /// Sets up the view model to show an existing trip.
        /// </summary>
        /// <param name="trip"></param>
        internal void ShowTrip(Trip trip)
        {
            showDelete = true;
            Trip = trip;
        }

        /// <summary>
        /// Sets up the view model to create a new trip.
        /// </summary>
        internal void NewTrip()
        {
            showDelete = false;
            Trip = new Trip();
        }
    }
}
