using SDKTemplate.Common;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private readonly NavigationHelper navigationHelper;
        private ItemList defaultViewModel;
        public string index = "index";

        public MainPage()
        {
            this.InitializeComponent();

            this.NavigationCacheMode = NavigationCacheMode.Required;

            this.navigationHelper = new NavigationHelper(this);
            this.defaultViewModel = new ItemList();

            this.navigationHelper.LoadState += this.NavigationHelper_Load;
            this.navigationHelper.SaveState += this.NavigationHelper_Save;
        }

        /// <summary>
        /// Gets the <see cref="NavigationHelper"/> associated with this <see cref="Page"/>.
        /// </summary>
        public NavigationHelper NavigationHelper
        {
            get { return this.navigationHelper; }
        }

        /// <summary>
        /// Gets the view model for this <see cref="Page"/>.
        /// This can be changed to a strongly typed view model.
        /// </summary>
        public ItemList DefaultViewModel
        {
            get { return this.defaultViewModel; }
        }

        ///<summary>
        ///Invoked when an item within a section is clicked
        ///Only the following types can be serialized by Frame.GetNavigationState: numbers, characters, strings, guids and boolean. 
        ///A frame that needs to be serialized cannot pass other types as the navigation param in Frame.Navigate.
        ///</summary>
        private void ListView_ItemClick(object sender, ItemClickEventArgs e)
        {
            this.Frame.Navigate(typeof(SubPage), e.ClickedItem.ToString());
        }

        ///<summary>
        ///Adds an item to the list when the app bar button is clicked
        ///</summary>
        private void AddItem()
        {
            this.defaultViewModel.Add(new Item() { Name = "Item " + (this.defaultViewModel.Count + 1).ToString() });
        }

        ///<summary>
        /// Resets the item list to the default.
        ///</summary>
        private void ResetItems()
        {
            this.defaultViewModel = ItemList.CreateDefaultItemList();
            list.ItemsSource = this.defaultViewModel;
        }

        /// <summary>
        /// Populates the page with content passed during navigation. Any saved state is also
        /// provided when recreating a page from a prior session.
        /// </summary>
        /// <param name="sender">
        /// The source of the event; typically <see cref="NavigationHelper"/>.
        /// </param>
        /// <param name="e">Event data that provides both the navigation parameter passed to
        /// <see cref="Frame.Navigate(Type, Object)"/> when this page was initially requested and
        /// a dictionary of state preserved by this page during an earlier
        /// session. The state will be null the first time a page is visited.</param>
        private void NavigationHelper_Load(object sender, LoadStateEventArgs e)
        {
            this.defaultViewModel = null;
            if (SuspensionManager.SessionState.ContainsKey(index))
            {
                this.defaultViewModel = SuspensionManager.SessionState[index] as ItemList;
            }

            if (this.defaultViewModel == null)
            {
                this.defaultViewModel = ItemList.CreateDefaultItemList();
            }

            list.ItemsSource = this.defaultViewModel;
        }

        /// <summary>
        /// Preserves state associated with this page in case the application is suspended or the
        /// page is discarded from the navigation cache. Values must conform to the serialization
        /// requirements of <see cref="SuspensionManager.SessionState"/>.
        /// </summary>
        /// <param name="sender">The source of the event; typically <see cref="NavigationHelper"/>.</param>
        /// <param name="e">Event data that provides an empty dictionary to be populated with
        /// serializable state.</param>
        private void NavigationHelper_Save(object sender, SaveStateEventArgs e)
        {
            SuspensionManager.SessionState[index] = defaultViewModel;
        }

        #region NavigationHelper registration

        /// <summary>
        /// The methods provided in this section are simply used to allow
        /// NavigationHelper to respond to the page's navigation methods.
        /// <para>
        /// Page specific logic should be placed in event handlers for the  
        /// <see cref="NavigationHelper.LoadState"/>
        /// and <see cref="NavigationHelper.SaveState"/>.
        /// The navigation parameter is available in the LoadState method 
        /// in addition to page state preserved during an earlier session.
        /// </para>
        /// </summary>
        /// <param name="e">Provides data for navigation methods and event
        /// handlers that cannot cancel the navigation request.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            this.navigationHelper.OnNavigatedTo(e);

            SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility =
                this.Frame.CanGoBack ? AppViewBackButtonVisibility.Visible : AppViewBackButtonVisibility.Collapsed;
        }
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            this.navigationHelper.OnNavigatedFrom(e);
        }
        #endregion
    }

    /// <summary>
    /// observable collection that will contain the list of items. List items are added when page is loaded.
    /// </summary>
    public class ItemList : ObservableCollection<Item>
    {
        public ItemList()
        {
        }

        static public ItemList CreateDefaultItemList()
        {
            return new ItemList()
            {
                new Item { Name = "Item 1" },
                new Item { Name = "Item 2" },
                new Item { Name = "Item 3" },
                new Item { Name = "Item 4" },
            };
        }
    }

    /// <summary>
    /// items that are in the observable collection (ItemList)
    /// </summary>
    public class Item
    {
        public string Name { get; set; }
        public override string ToString()
        {
            return this.Name;
        }
    }
}
