using System;
using System.Collections.ObjectModel;
using System.Windows.Input;
using Windows.Foundation.Metadata;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace AppUIBasics.ControlPages
{
    public class ListItemData
    {
        public String Text { get; set; }
        public ICommand Command { get; set; }
    }

    public sealed partial class UICommandPage : Page
    {
        ObservableCollection<ListItemData> collection = new ObservableCollection<ListItemData>();

        public UICommandPage()
        {
            this.InitializeComponent();
        }

        private void DeleteCommand_ExecuteRequested(XamlUICommand sender, ExecuteRequestedEventArgs args)
        {
            if (args.Parameter != null)
            {
                foreach (var i in collection)
                {
                    if (i.Text == (args.Parameter as string))
                    {
                        collection.Remove(i);
                        return;
                    }
                }
            }
            if (ListViewRight.SelectedIndex != -1)
            {
                collection.RemoveAt(ListViewRight.SelectedIndex);
            }
        }

        private void ListView_Loaded(object sender, RoutedEventArgs e)
        {
            var listView = (ListView)sender;
            listView.ItemsSource = collection;
        }

        private void ListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (ListViewRight.SelectedIndex != -1)
            {
                var item = collection[ListViewRight.SelectedIndex];
            }
        }

        private void ListViewSwipeContainer_PointerEntered(object sender, PointerRoutedEventArgs e)
        {
            if (e.Pointer.PointerDeviceType == Windows.Devices.Input.PointerDeviceType.Mouse || e.Pointer.PointerDeviceType == Windows.Devices.Input.PointerDeviceType.Pen)
            {
                VisualStateManager.GoToState(sender as Control, "HoverButtonsShown", true);
            }
        }

        private void ListViewSwipeContainer_PointerExited(object sender, PointerRoutedEventArgs e)
        {
            VisualStateManager.GoToState(sender as Control, "HoverButtonsHidden", true);
        }

        private void ControlExample_Loaded(object sender, RoutedEventArgs e)
        {
            if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 7))
            {
                var deleteCommand = new StandardUICommand(StandardUICommandKind.Delete);
                deleteCommand.ExecuteRequested += DeleteCommand_ExecuteRequested;

                DeleteFlyoutItem.Command = deleteCommand;

                for (var i = 0; i < 15; i++)
                {
                    collection.Add(new ListItemData { Text = "List item " + i.ToString(), Command = deleteCommand });
                }
            }
            else
            {
                for (var i = 0; i < 15; i++)
                {
                    collection.Add(new ListItemData { Text = "List item " + i.ToString(), Command = null });
                }
            }
        }
    }
}
