using AppUIBasics.Common;
using AppUIBasics.Data;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class NewControlsPage : Page, INotifyPropertyChanged
    {
        private List<ControlInfoDataItem> _items;
        private string itemId;

        public List<ControlInfoDataItem> Items
        {
            get { return _items; }
            set { SetProperty(ref _items, value); }
        }

        public NewControlsPage()
        {
            if (NavigationRootPage.Current.Groups == null || NavigationRootPage.Current.Groups.Count() == 0)
            {
                NavigationRootPage.Current.GroupsLoaded += RootPage_GroupsLoaded;
            }
            else
            {
                RootPage_GroupsLoaded(NavigationRootPage.Current, new EventArgs());
            }
            this.InitializeComponent();
        }

        private void RootPage_GroupsLoaded(object sender, EventArgs e)
        {
            var controlInfoDataGroups = NavigationRootPage.Current.Groups;

            List<ControlInfoDataItem> items = new List<ControlInfoDataItem>();
            foreach (ControlInfoDataGroup group in controlInfoDataGroups)
            {
                foreach (ControlInfoDataItem item in group.Items.Where(i => i.IsNew))
                {
                    items.Add(item);
                }
            }

            Items = items.OrderBy(item => item.Title).ToList();
        }

        void ItemView_ItemClick(object sender, ItemClickEventArgs e)
        {
            itemId = ((ControlInfoDataItem)e.ClickedItem).UniqueId;
            var container = itemGridView.ContainerFromItem(e.ClickedItem) as GridViewItem;
            if (container != null)
            {
                itemGridView.PrepareConnectedAnimation("controlAnimation", (ControlInfoDataItem)e.ClickedItem, "controlRoot");
            }
            this.Frame.Navigate(typeof(ItemPage), itemId);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
            NavigationRootPage.Current.NavigationView.AlwaysShowHeader = false;
            NavigationRootPage.Current.NavigationView.Header = "New controls";
            NavigationRootPage.Current.NewControlsMenuItem.IsSelected = true;
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            NavigationRootPage.Current.NavigationView.AlwaysShowHeader = true;
            base.OnNavigatingFrom(e);
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private bool SetProperty<T>(ref T storage, T value, [CallerMemberName] String propertyName = null)
        {
            if (object.Equals(storage, value)) return false;

            storage = value;
            this.OnPropertyChanged(propertyName);
            return true;
        }

        private void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            this.PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        async private void itemGridView_Loaded(object sender, RoutedEventArgs e)
        {
            if (itemId != null)
            {
                var item = Items.First(s => s.UniqueId == itemId);
                itemGridView.ScrollIntoView(item);
                if (NavigationRootPage.Current.IsFocusSupported)
                {
                    ((GridViewItem)itemGridView.ContainerFromItem(item))?.Focus(FocusState.Programmatic);
                }
                ConnectedAnimation animation =
                    ConnectedAnimationService.GetForCurrentView().GetAnimation("controlAnimation");
                if (animation != null)
                {
                    await itemGridView.TryStartConnectedAnimationAsync(
                        animation, item, "controlRoot");
                }
            }
        }

        private void itemGridView_KeyDown(object sender, Windows.UI.Xaml.Input.KeyRoutedEventArgs e)
        {
            if (e.Key == Windows.System.VirtualKey.Up)
            {
                var nextElement = FocusManager.FindNextElement(FocusNavigationDirection.Up);
                if (nextElement.GetType() == typeof(NavigationViewItem))
                {
                    pageHeader.Focus(FocusState.Programmatic);
                }
                else
                {
                    FocusManager.TryMoveFocus(FocusNavigationDirection.Up);
                }
            }
        }
    }
}
