using AppUIBasics.Data;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ParallaxViewPage : Page, INotifyPropertyChanged
    {
        private List<ControlInfoDataItem> _items;

        public ParallaxViewPage()
        {
            this.InitializeComponent();
        }

        public List<ControlInfoDataItem> Items
        {
            get { return _items; }
            set { SetProperty(ref _items, value); }
        }

        protected async override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);

            IEnumerable<ControlInfoDataGroup> groups = await ControlInfoDataSource.GetGroupsAsync();
            List<ControlInfoDataItem> items = new List<ControlInfoDataItem>();
            foreach (ControlInfoDataGroup group in groups)
            {
                foreach (ControlInfoDataItem item in group.Items)
                {
                    items.Add(item);
                }
            }
            Items = items.OrderBy(item => item.Title).ToList();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private bool SetProperty<T>(ref T storage, T value, [CallerMemberName] String propertyName = null)
        {
            if (Equals(storage, value)) return false;

            storage = value;
            this.OnPropertyChanged(propertyName);
            return true;
        }

        private void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            this.PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
