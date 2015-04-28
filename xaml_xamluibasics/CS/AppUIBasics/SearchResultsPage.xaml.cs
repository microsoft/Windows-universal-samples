using AppUIBasics.Common;
using AppUIBasics.Data;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Search Results Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234240

namespace AppUIBasics
{
    /// <summary>
    /// This page displays search results when a global search is directed to this application.
    /// </summary>
    public sealed partial class SearchResultsPage : Page, INotifyPropertyChanged
    {
        private NavigationHelper navigationHelper;
        private string _queryText;
        private List<Filter> _filters;
        private bool _showFilters;
        private List<ControlInfoDataItem> _results;


        public string QueryText
        {
            get { return _queryText; }
            set { this.SetProperty(ref _queryText, value); }
        }

        public List<Filter> Filters
        {
            get { return _filters; }
            set { this.SetProperty(ref _filters, value); }
        }

        public bool ShowFilters
        {
            get { return _showFilters; }
            set { this.SetProperty(ref _showFilters, value); }
        }

        public List<ControlInfoDataItem> Results
        {
            get { return _results; }
            set { this.SetProperty(ref _results, value); }
        }

        /// <summary>
        /// NavigationHelper is used on each page to aid in navigation and 
        /// process lifetime management
        /// </summary>
        public NavigationHelper NavigationHelper
        {
            get { return this.navigationHelper; }
        }

        public SearchResultsPage()
        {
            this.InitializeComponent();
            this.navigationHelper = new NavigationHelper(this);
            this.navigationHelper.LoadState += navigationHelper_LoadState;
        }

        public Dictionary<String, IEnumerable<AppUIBasics.Data.ControlInfoDataItem>> FullResults
        {
            get;
            set;
        }

        /// <summary>
        /// Populates the page with content passed during navigation.  Any saved state is also
        /// provided when recreating a page from a prior session.
        /// </summary>
        /// <param name="navigationParameter">The parameter value passed to
        /// <see cref="Frame.Navigate(Type, Object)"/> when this page was initially requested.
        /// </param>
        /// <param name="pageState">A dictionary of state preserved by this page during an earlier
        /// session.  This will be null the first time a page is visited.</param>
        private async void navigationHelper_LoadState(object sender, LoadStateEventArgs e)
        {
            var queryText = e.NavigationParameter as String;

            if (!string.IsNullOrEmpty(queryText))
            {
                // Application-specific searching logic.  The search process is responsible for
                // creating a list of user-selectable result categories:
                var filterList = new List<Filter>();
                var totalMatchingItems = 0;

                FullResults = new Dictionary<string, IEnumerable<Data.ControlInfoDataItem>>();
                var groups = await AppUIBasics.Data.ControlInfoDataSource.GetGroupsAsync();

                foreach (var group in groups)
                {
                    var matchingItems = group.Items.Where(
                        item => item.Title.IndexOf(queryText, StringComparison.CurrentCultureIgnoreCase) >= 0
                            || item.Subtitle.IndexOf(queryText, StringComparison.CurrentCultureIgnoreCase) >= 0);
                    int numberOfMatchingItems = matchingItems.Count();
                    if (numberOfMatchingItems > 0)
                    {
                        FullResults.Add(group.Title, matchingItems);
                        filterList.Add(new Filter(group.Title, numberOfMatchingItems));
                    }
                    totalMatchingItems += numberOfMatchingItems;
                }

                filterList.Insert(0, new Filter("All", totalMatchingItems, true));

                // Communicate results through the view model
                QueryText = '\u201c' + queryText + '\u201d';
                Filters = filterList;
                ShowFilters = filterList.Count > 1;
                if (FullResults.Count() < 1)
                {
                    // Display informational text when there are no search results.
                    VisualStateManager.GoToState(this, "NoResultsFound", true);
                }
            }
            else
            {
                var isFocused = query.Focus(FocusState.Programmatic);
            }
        }

        /// <summary>
        /// Invoked when a filter is selected using a RadioButton when not snapped.
        /// </summary>
        /// <param name="sender">The selected RadioButton instance.</param>
        /// <param name="e">Event data describing how the RadioButton was selected.</param>
        void Filter_Checked(object sender, RoutedEventArgs e)
        {
            var filter = (sender as FrameworkElement).DataContext;

            // Determine what filter was selected
            var selectedFilter = filter as Filter;
            if (selectedFilter != null)
            {
                // Mirror the results into the corresponding Filter object to allow the
                // RadioButton representation used when not snapped to reflect the change
                selectedFilter.Active = true;
                
                if (selectedFilter.Name.Equals("All"))
                {
                    var tempResults = new List<AppUIBasics.Data.ControlInfoDataItem>();
                    foreach (var group in FullResults)
                    {
                        tempResults.AddRange(group.Value);

                    }
                    Results = tempResults;
                }
                else if (FullResults.ContainsKey(selectedFilter.Name))
                {
                    Results =
                      new List<AppUIBasics.Data.ControlInfoDataItem>(FullResults[selectedFilter.Name]);
                }

                // Ensure results are found
                if (Results.Count > 0)
                {
                    VisualStateManager.GoToState(this, "ResultsFound", true);
                    return;
                }
            }

            // Display informational text when there are no search results.
            VisualStateManager.GoToState(this, "NoResultsFound", true);
        }

        public async static void SearchBox_SuggestionsRequested(SearchBox sender, SearchBoxSuggestionsRequestedEventArgs args)
        {

            // This object lets us edit the SearchSuggestionCollection asynchronously. 
            var deferral = args.Request.GetDeferral();
            try
            {

                var suggestions = args.Request.SearchSuggestionCollection;
                var groups = await AppUIBasics.Data.ControlInfoDataSource.GetGroupsAsync();

                foreach (var group in groups)
                {
                    var matchingItems = group.Items.Where(
                        item => item.Title.StartsWith(args.QueryText, StringComparison.CurrentCultureIgnoreCase));

                    foreach (var item in matchingItems)
                    {
                        suggestions.AppendQuerySuggestion(item.Title);
                    }
                }

                foreach (string alternative in args.LinguisticDetails.QueryTextAlternatives)
                {
                    if (alternative.StartsWith(
                        args.QueryText, StringComparison.CurrentCultureIgnoreCase))
                    {
                        suggestions.AppendQuerySuggestion(alternative);
                    }
                }

            }
            finally
            {
                deferral.Complete();
            }

        }

        private async void query_TextChanged(object sender, TextChangedEventArgs e)
        {
            var filterList = new List<Filter>();
            var totalMatchingItems = 0;
            var groups = await AppUIBasics.Data.ControlInfoDataSource.GetGroupsAsync();
            FullResults = new Dictionary<string, IEnumerable<Data.ControlInfoDataItem>>();

            foreach (var group in groups)
            {
                var matchingItems = group.Items.Where(
                    item => item.Title.IndexOf(query.Text, StringComparison.CurrentCultureIgnoreCase) >= 0
                        || item.Subtitle.IndexOf(query.Text, StringComparison.CurrentCultureIgnoreCase) >= 0);
                int numberOfMatchingItems = matchingItems.Count();
                if (numberOfMatchingItems > 0)
                {
                    FullResults.Add(group.Title, matchingItems);
                    filterList.Add(new Filter(group.Title, numberOfMatchingItems));
                }
                totalMatchingItems += numberOfMatchingItems;
            }

            filterList.Insert(0, new Filter("All", totalMatchingItems, true));

            // Communicate results through the view model
            QueryText = '\u201c' + query.Text + '\u201d';
            Filters = filterList;
            ShowFilters = filterList.Count > 1;
            if (FullResults.Count() < 1)
            {
                // Display informational text when there are no search results.
                VisualStateManager.GoToState(this, "NoResultsFound", true);
            }
        }

        private void resultsGridView_ItemClick(object sender, ItemClickEventArgs e)
        {
            this.Frame.Navigate(
                typeof(ItemPage),
                ((AppUIBasics.Data.ControlInfoDataItem)e.ClickedItem).UniqueId);
        }

        private void SearchBox_QuerySubmitted(SearchBox sender, SearchBoxQuerySubmittedEventArgs args)
        {
            this.Frame.Navigate(typeof(SearchResultsPage), args.QueryText);
        }

        #region NavigationHelper registration

        /// The methods provided in this section are simply used to allow
        /// NavigationHelper to respond to the page's navigation methods.
        /// 
        /// Page specific logic should be placed in event handlers for the  
        /// <see cref="GridCS.Common.NavigationHelper.LoadState"/>
        /// and <see cref="GridCS.Common.NavigationHelper.SaveState"/>.
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
            this.OnPropertyChanged(propertyName);
            return true;
        }

        /// <summary>
        /// Notifies listeners that a property value has changed.
        /// </summary>
        /// <param name="propertyName">Name of the property used to notify listeners.  This
        /// value is optional and can be provided automatically when invoked from compilers
        /// that support <see cref="CallerMemberNameAttribute"/>.</param>
        private void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            var eventHandler = this.PropertyChanged;
            if (eventHandler != null)
            {
                eventHandler(this, new PropertyChangedEventArgs(propertyName));
            }
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

        public Filter(String name, int count, bool active = false)
        {
            this.Name = name;
            this.Count = count;
            this.Active = active;
        }

        public override String ToString()
        {
            return Description;
        }

        public String Name
        {
            get { return _name; }
            set { if (this.SetProperty(ref _name, value)) this.OnPropertyChanged("Description"); }
        }

        public int Count
        {
            get { return _count; }
            set { if (this.SetProperty(ref _count, value)) this.OnPropertyChanged("Description"); }
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
            this.OnPropertyChanged(propertyName);
            return true;
        }

        /// <summary>
        /// Notifies listeners that a property value has changed.
        /// </summary>
        /// <param name="propertyName">Name of the property used to notify listeners.  This
        /// value is optional and can be provided automatically when invoked from compilers
        /// that support <see cref="CallerMemberNameAttribute"/>.</param>
        private void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            var eventHandler = this.PropertyChanged;
            if (eventHandler != null)
            {
                eventHandler(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}
