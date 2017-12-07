using ListViewSample.Model;
using System;
using System.Collections.ObjectModel;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace ListViewSample
{
    public sealed partial class ScrollIntoViewSample : Page
    {
        private ObservableCollection<Item> _items;
        private SolidColorBrush InvalidFormat = new SolidColorBrush(Colors.Red);
        private SolidColorBrush ValidFormat = new SolidColorBrush(Colors.Green);
        public ScrollIntoViewSample()
        {
            this.InitializeComponent();
            _items = Item.GetItems(1000);
        }

        private void ShowSplitView(object sender, RoutedEventArgs e)
        {      
            MySamplesPane.SamplesSplitView.IsPaneOpen = !MySamplesPane.SamplesSplitView.IsPaneOpen;
        }

        private void Scroll_Click(object sender, RoutedEventArgs e)
        {
            int numVal;
            if (Int32.TryParse(scrollId.Text, out numVal) && numVal >= 0 && numVal < ItemsListView.Items.Count)
            {
                var item = (ComboBoxItem)ScrollAlignment.SelectedItem;
                var Alignment = (ScrollIntoViewAlignment)item.Tag;
                ItemsListView.ScrollIntoView(ItemsListView.Items[numVal], Alignment);
                scrollId.BorderBrush = ValidFormat;
            }
            else
            {
                scrollId.BorderBrush = InvalidFormat;
            }
        }
    }
}
