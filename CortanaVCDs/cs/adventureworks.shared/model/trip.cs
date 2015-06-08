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
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventureWorks.Model
{
    /// <summary>
    /// Trip model. Stores basic details about individual trips, retreived from the TripStore.
    /// Implements INotifyPropertyChanged to allow two-way data-binding at the UI layer.
    /// </summary>
    public class Trip : INotifyPropertyChanged
    {
        private string destination;
        private string description;
        private DateTime? startDate;
        private DateTime? endDate;
        private string notes;

        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Trip Destination. Should not be an empty string.
        /// </summary>
        public string Destination
        {
            get
            {
                return destination;
            }
            set
            {
                destination = value;
                NotifyPropertyChanged("Destination");
            }
        }

        /// <summary>
        /// Description of a trip. Shown to users as a basic description in the UI.
        /// </summary>
        public string Description
        {
            get
            {
                return description;
            }
            set
            {
                description = value;
                NotifyPropertyChanged("Description");
            }
        }

        /// <summary>
        /// The beginning date of a trip.
        /// </summary>
        public DateTime? StartDate
        {
            get
            {
                return startDate;
            }
            set
            {
                startDate = value;
                NotifyPropertyChanged("StartDate");
            }
        }

        /// <summary>
        /// The ending date of a trip.
        /// </summary>
        public DateTime? EndDate
        {
            get
            {
                return endDate;
            }
            set
            {
                endDate = value;
                NotifyPropertyChanged("EndDate");
            }
        }

        /// <summary>
        /// Any notes about a trip, such as things to take, activities, etc.
        /// </summary>
        public string Notes
        {
            get
            {
                return notes;
            }
            set
            {
                notes = value;
                NotifyPropertyChanged("Notes");
            }
        }

        /// <summary>
        /// Notify any subscribers to the INotifyPropertyChanged interface that a property
        /// was updated. This allows the UI to automatically update (for instance, if Cortana
        /// triggers an update to a trip, or removal of a trip).
        /// </summary>
        /// <param name="propertyName">The case-sensitive name of the property that was updated.</param>
        private void NotifyPropertyChanged(string propertyName)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                PropertyChangedEventArgs args = new PropertyChangedEventArgs(propertyName);
                handler(this, args);
            }
        }
    }
}
