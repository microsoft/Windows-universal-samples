//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using AppUIBasics.Common;
using AppUIBasics.Data;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace AppUIBasics
{
    /// <summary>
    /// This page displays search results when a global search is directed to this application.
    /// </summary>
    public sealed partial class SearchResultsPage : ItemsPageBase
    {
        private IEnumerable<Filter> _filters;
        private int? _pivotIndex;
        string _queryText;

        public IEnumerable<Filter> Filters
        {
            get { return _filters; }
            set { this.SetProperty(ref _filters, value); }
        }

        public SearchResultsPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);

            var queryText = e.Parameter?.ToString().ToLower();

            BuildFilterList(queryText);

            NavigationRootPage.Current.NavigationView.Header = "Search";
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            base.OnNavigatingFrom(e);

            _pivotIndex = resultsPivot.SelectedIndex != -1 ? resultsPivot.SelectedIndex : default(int?);
        }

        private void OnResultsPivotLoaded(object sender, RoutedEventArgs e)
        {
            if (NavigationRootPage.Current.DeviceFamily == DeviceType.Xbox)
            {
                resultsPivot.IsHeaderItemsCarouselEnabled = false;
            }

            resultsPivot.Focus(FocusState.Programmatic);
        }

        private void BuildFilterList(string queryText)
        {
            if (!string.IsNullOrEmpty(queryText))
            {
                // Application-specific searching logic.  The search process is responsible for
                // creating a list of user-selectable result categories:
                var filterList = new List<Filter>();

                foreach (var group in ControlInfoDataSource.Instance.Groups)
                {
                    var matchingItems =
                        group.Items.Where(item =>
                            item.Title.ToLower().Contains(queryText) ||
                            item.Subtitle.ToLower().Contains(queryText))
                        .ToList();

                    int numberOfMatchingItems = matchingItems.Count();

                    if (numberOfMatchingItems > 0)
                    {
                        filterList.Add(new Filter(group.Title, numberOfMatchingItems, matchingItems));
                    }
                }

                if (filterList.Count == 0)
                {
                    // Display informational text when there are no search results.
                    VisualStateManager.GoToState(this, "NoResultsFound", false);
                    var textbox = NavigationRootPage.Current.PageHeader.GetDescendantsOfType<AutoSuggestBox>().FirstOrDefault();
                    textbox?.Focus(FocusState.Programmatic);
                }
                else
                {
                    // When there are search results, set Filters
                    var allControls = filterList.SelectMany(s => s.Items).ToList();
                    filterList.Insert(0, new Filter("All", allControls.Count, allControls, true));
                    Filters = filterList;

                    // Check to see if the current query matches the last
                    if (_queryText == queryText)
                    {
                        // If so try to restore any previously selected pivot item
                        if (_pivotIndex != null)
                        {
                            resultsPivot.SelectedIndex = _pivotIndex.Value;
                        }
                    }
                    else
                    {
                        // Otherwise reset query text and pivot index
                        _queryText = queryText;
                        _pivotIndex = null;
                    }

                    VisualStateManager.GoToState(this, "ResultsFound", false);
                }
            }
        }

        protected override bool GetIsNarrowLayoutState()
        {
            return LayoutVisualStates.CurrentState == NarrowLayout;
        }
    }

    /// <summary>
    /// View model describing one of the filters available for viewing search results.
    /// </summary>
    public sealed class Filter : INotifyPropertyChanged
    {
        private String _name;
        private int _count;
        private bool? _active;
        private List<ControlInfoDataItem> _items;

        public Filter(String name, int count, List<ControlInfoDataItem> controlInfoList, bool active = false)
        {
            this.Name = name;
            this.Count = count;
            this.Active = active;
            this.Items = controlInfoList;
        }

        public override String ToString()
        {
            return Description;
        }

        public List<ControlInfoDataItem> Items
        {
            get { return _items; }
            set { this.SetProperty(ref _items, value); }
        }

        public String Name
        {
            get { return _name; }
            set { if (this.SetProperty(ref _name, value)) this.NotifyPropertyChanged(nameof(Description)); }
        }

        public int Count
        {
            get { return _count; }
            set { if (this.SetProperty(ref _count, value)) this.NotifyPropertyChanged(nameof(Description)); }
        }

        public bool? Active
        {
            get { return _active; }
            set { this.SetProperty(ref _active, value); }
        }

        public String Description
        {
            get { return String.Format("{0} ({1})", _name, _count); }
        }

        /// <summary>
        /// Multicast event for property change notifications.
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Checks if a property already matches a desired value.  Sets the property and
        /// notifies listeners only when necessary.
        /// </summary>
        /// <typeparam name="T">Type of the property.</typeparam>
        /// <param name="storage">Reference to a property with both getter and setter.</param>
        /// <param name="value">Desired value for the property.</param>
        /// <param name="propertyName">Name of the property used to notify listeners.  This
        /// value is optional and can be provided automatically when invoked from compilers that
        /// support CallerMemberName.</param>
        /// <returns>True if the value was changed, false if the existing value matched the
        /// desired value.</returns>
        private bool SetProperty<T>(ref T storage, T value, [CallerMemberName] String propertyName = null)
        {
            if (object.Equals(storage, value)) return false;

            storage = value;
            this.NotifyPropertyChanged(propertyName);
            return true;
        }

        /// <summary>
        /// Notifies listeners that a property value has changed.
        /// </summary>
        /// <param name="propertyName">Name of the property used to notify listeners.  This
        /// value is optional and can be provided automatically when invoked from compilers
        /// that support <see cref="CallerMemberNameAttribute"/>.</param>
        private void NotifyPropertyChanged([CallerMemberName] string propertyName = null)
        {
            this.PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}