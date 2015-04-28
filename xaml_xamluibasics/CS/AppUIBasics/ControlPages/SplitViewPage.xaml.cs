using AppUIBasics.Common;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class SplitViewPage : Page
    {
        private ObservableCollection<NavLink> _navLinks =  new ObservableCollection<NavLink>()
        {
            new NavLink() { Label = "People", Symbol = Windows.UI.Xaml.Controls.Symbol.People  },
            new NavLink() { Label = "Globe", Symbol = Windows.UI.Xaml.Controls.Symbol.Globe },
            new NavLink() { Label = "Message", Symbol = Windows.UI.Xaml.Controls.Symbol.Message },
            new NavLink() { Label = "Mail", Symbol = Windows.UI.Xaml.Controls.Symbol.Mail },
        };

        public ObservableCollection<NavLink> NavLinks
        {
            get { return _navLinks; }
        }

        public SplitViewPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);

            
        }

        private void togglePaneButton_Click(object sender, RoutedEventArgs e)
        {
            if (Window.Current.Bounds.Width >= 640)
            {
                if (splitView.IsPaneOpen)
                {
                    splitView.DisplayMode = SplitViewDisplayMode.CompactOverlay;
                    splitView.IsPaneOpen = false;
                }
                else
                {
                    splitView.IsPaneOpen = true;
                    splitView.DisplayMode = SplitViewDisplayMode.Inline;
                }
            }
            else
            {
                splitView.IsPaneOpen = !splitView.IsPaneOpen;
            }
        }

        private void PanePlacement_Toggled(object sender, RoutedEventArgs e)
        {
            var ts = sender as ToggleSwitch;
            if (ts.IsOn)
            {
                splitView.PanePlacement = SplitViewPanePlacement.Right;
            }
            else
            {
                splitView.PanePlacement = SplitViewPanePlacement.Left;
            }
        }

        private void NavLinksList_ItemClick(object sender, ItemClickEventArgs e)
        {
            content.Text = (e.ClickedItem as NavLink).Label + " Page";
        }

        private void displayModeCombobox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            splitView.DisplayMode = (SplitViewDisplayMode)Enum.Parse(typeof(SplitViewDisplayMode), (e.AddedItems[0] as ComboBoxItem).Content.ToString());
        }

        private void paneBackgroundCombobox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var item = (e.AddedItems[0] as ComboBoxItem).Content;
            if (item is SolidColorBrush)
                splitView.PaneBackground = item as SolidColorBrush;
            else
            {
                var hex = item.ToString().Substring(1);
                var a = byte.Parse(hex.Substring(0, 2), System.Globalization.NumberStyles.HexNumber);
                var r = byte.Parse(hex.Substring(2, 2), System.Globalization.NumberStyles.HexNumber);
                var g = byte.Parse(hex.Substring(4, 2), System.Globalization.NumberStyles.HexNumber);
                var b = byte.Parse(hex.Substring(6, 2), System.Globalization.NumberStyles.HexNumber);
                var brush = new SolidColorBrush(Color.FromArgb(a, r, g, b));
                splitView.PaneBackground = brush;
            }
        }
    }

    public class NavLink
    {
        public string Label { get; set; }
        public Symbol Symbol { get; set; }
    }
}
