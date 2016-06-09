using ListViewSample.Model;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using System.Diagnostics;
using System.Collections.ObjectModel;
using System.Linq;

namespace ListViewSample
{
    public sealed partial class SimpleListViewSample : Page
    {
        public SimpleListViewSample()
        {
            this.InitializeComponent();
            ContactsCVS.Source = Contact.GetContactsGrouped(250);
        }
        private void ShowSliptView(object sender, RoutedEventArgs e)
        {
            MySamplesPane.SamplesSplitView.IsPaneOpen = !MySamplesPane.SamplesSplitView.IsPaneOpen;
        }

        private void Grid_Tapped(object sender, Windows.UI.Xaml.Input.TappedRoutedEventArgs e)
        {
            Grid grid = (Grid)sender;
            TextBlock tb = (TextBlock)grid.Children[0];
            //TextBlock collapseIcon = (TextBlock)grid.Children[1];
            string key = tb.Text;
            Debug.WriteLine(key);
            var grp = ((ObservableCollection<GroupInfoList>)ContactsCVS.Source).FirstOrDefault(tGrp => tGrp.Key.ToString() == key);
            if (grp != null)
            {
                Visibility visibility = (grp.isGroupCollapsed) ? Visibility.Visible : Visibility.Collapsed;
                //collapseIcon.Text = (grp.isGroupCollapsed) ? "-" : "+";
                grp.isGroupCollapsed = !grp.isGroupCollapsed;
                foreach (var item in grp)
                {
                    var container = (ListViewItem)groupView.ContainerFromItem(item);
                    container.Visibility = visibility;
                }
            }
        }
    }
}
