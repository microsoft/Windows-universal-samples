using AppUIBasics.SamplePages;
using AppUIBasics.Common;
using System.Collections.ObjectModel;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Diagnostics;
using Windows.Foundation;
using Windows.Foundation.Metadata;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Animation;
using System.Linq;
using Windows.UI.Xaml.Input;
using Windows.System;
using Windows.UI.ViewManagement;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;
using AppUIBasics.Data;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class NavigationViewPage : Page
    {
        public static Boolean CameFromToggle = false;

        public static Boolean CameFromGridChange = false;

        public VirtualKey ArrowKey;

        public ObservableCollection<CategoryBase> Categories { get; set; }

        public NavigationViewPage()
        {
            this.InitializeComponent();

            nvSample2.SelectedItem = nvSample2.MenuItems.OfType<Microsoft.UI.Xaml.Controls.NavigationViewItem>().First();
            nvSample5.SelectedItem = nvSample5.MenuItems.OfType<Microsoft.UI.Xaml.Controls.NavigationViewItem>().First();
            nvSample6.SelectedItem = nvSample6.MenuItems.OfType<Microsoft.UI.Xaml.Controls.NavigationViewItem>().First();
            nvSample7.SelectedItem = nvSample7.MenuItems.OfType<Microsoft.UI.Xaml.Controls.NavigationViewItem>().First();

            Categories = new ObservableCollection<CategoryBase>();
            Category firstCategory = new Category { Name = "Category 1", Glyph = Symbol.Home, Tooltip = "This is category 1" };
            Categories.Add(firstCategory);
            Categories.Add(new Category { Name = "Category 2", Glyph = Symbol.Keyboard, Tooltip = "This is category 2" });
            Categories.Add(new Category { Name = "Category 3", Glyph = Symbol.Library, Tooltip = "This is category 3" });
            Categories.Add(new Category { Name = "Category 4", Glyph = Symbol.Mail, Tooltip = "This is category 4" });
            nvSample4.SelectedItem = firstCategory;
        }

        public Microsoft.UI.Xaml.Controls.NavigationViewPaneDisplayMode ChoosePanePosition(Boolean toggleOn)
        {
            if (toggleOn)
            {
                return Microsoft.UI.Xaml.Controls.NavigationViewPaneDisplayMode.Left;
            } else
            {
                return Microsoft.UI.Xaml.Controls.NavigationViewPaneDisplayMode.Top;
            }
        }

        private void NavigationView_SelectionChanged(Microsoft.UI.Xaml.Controls.NavigationView sender, Microsoft.UI.Xaml.Controls.NavigationViewSelectionChangedEventArgs args)
        {
            if (args.IsSettingsSelected)
            {
                contentFrame.Navigate(typeof(SampleSettingsPage));
            }
            else
            {
                var selectedItem = (Microsoft.UI.Xaml.Controls.NavigationViewItem)args.SelectedItem;
                string selectedItemTag = ((string)selectedItem.Tag);
                sender.Header = "Sample Page " + selectedItemTag.Substring(selectedItemTag.Length - 1);
                string pageName = "AppUIBasics.SamplePages." + selectedItemTag;
                Type pageType = Type.GetType(pageName);
                contentFrame.Navigate(pageType);
            }
        }

        private void NavigationView_SelectionChanged2(Microsoft.UI.Xaml.Controls.NavigationView sender, Microsoft.UI.Xaml.Controls.NavigationViewSelectionChangedEventArgs args)
        {
            if (!CameFromGridChange)
            {
                if (args.IsSettingsSelected)
                {
                    contentFrame2.Navigate(typeof(SampleSettingsPage));
                }
                else
                {
                    var selectedItem = (Microsoft.UI.Xaml.Controls.NavigationViewItem)args.SelectedItem;
                    string selectedItemTag = ((string)selectedItem.Tag);
                    string pageName = "AppUIBasics.SamplePages." + selectedItemTag;
                    Type pageType = Type.GetType(pageName);
                    contentFrame2.Navigate(pageType);
                }
            }
            
            CameFromGridChange = false;
        }

        private void NavigationView_SelectionChanged4(Microsoft.UI.Xaml.Controls.NavigationView sender, Microsoft.UI.Xaml.Controls.NavigationViewSelectionChangedEventArgs args)
        {
            if (args.IsSettingsSelected)
            {
                contentFrame4.Navigate(typeof(SampleSettingsPage));
            }
            else
            {
                Debug.WriteLine("Before hitting sample page 1");

                var selectedItem = (Category)args.SelectedItem;
                string selectedItemTag = selectedItem.Name;
                sender.Header = "Sample Page " + selectedItemTag.Substring(selectedItemTag.Length - 1);
                string pageName = "AppUIBasics.SamplePages." + "SamplePage1";
                Type pageType = Type.GetType(pageName);
                contentFrame4.Navigate(pageType);
            }
        }


        private void NavigationView_SelectionChanged5(Microsoft.UI.Xaml.Controls.NavigationView sender, Microsoft.UI.Xaml.Controls.NavigationViewSelectionChangedEventArgs args)
        {
            if (args.IsSettingsSelected)
            {
                contentFrame5.Navigate(typeof(SampleSettingsPage));
            }
            else
            {
                var selectedItem = (Microsoft.UI.Xaml.Controls.NavigationViewItem)args.SelectedItem;
                string selectedItemTag = ((string)selectedItem.Tag);
                sender.Header = "Sample Page " + selectedItemTag.Substring(selectedItemTag.Length - 1);
                string pageName = "AppUIBasics.SamplePages." + selectedItemTag;
                Type pageType = Type.GetType(pageName);
                contentFrame5.Navigate(pageType);
            }
        }
        private void NavigationView_SelectionChanged6(Microsoft.UI.Xaml.Controls.NavigationView sender, Microsoft.UI.Xaml.Controls.NavigationViewSelectionChangedEventArgs args)
        {
            if (args.IsSettingsSelected)
            {
                contentFrame6.Navigate(typeof(SampleSettingsPage));
            }
            else
            {
                var selectedItem = (Microsoft.UI.Xaml.Controls.NavigationViewItem)args.SelectedItem;
                string pageName = "AppUIBasics.SamplePages." + ((string)selectedItem.Tag);
                Type pageType = Type.GetType(pageName);
                contentFrame6.Navigate(pageType);
            }
        }

        private void NavigationView_SelectionChanged7(Microsoft.UI.Xaml.Controls.NavigationView sender, Microsoft.UI.Xaml.Controls.NavigationViewSelectionChangedEventArgs args)
        {
            if (args.IsSettingsSelected)
            {
                contentFrame7.Navigate(typeof(SampleSettingsPage));
            }
            else
            {
                var selectedItem = (Microsoft.UI.Xaml.Controls.NavigationViewItem)args.SelectedItem;
                string pageName = "AppUIBasics.SamplePages." + ((string)selectedItem.Tag);
                Type pageType = Type.GetType(pageName);

                contentFrame7.Navigate(pageType, null, args.RecommendedNavigationTransitionInfo);
            }
        }
 
        private void databindHeader_Checked(object sender, RoutedEventArgs e)
        {
            Categories = new ObservableCollection<CategoryBase>()
            {
                new Header { Name = "Header1 "},
                new Category { Name = "Category 1", Glyph = Symbol.Home, Tooltip = "This is category 1" },
                new Category { Name = "Category 2", Glyph = Symbol.Keyboard, Tooltip = "This is category 2" },
                new Separator(),
                new Header { Name = "Header2 "},
                new Category {Name = "Category 3", Glyph = Symbol.Library, Tooltip = "This is category 3" },
                new Category {Name = "Category 4", Glyph = Symbol.Mail, Tooltip = "This is category 3" }
            };
        }

        private void databindHeader_Checked_Unchecked(object sender, RoutedEventArgs e)
        {
            Categories = new ObservableCollection<CategoryBase>()
            {
                new Category { Name = "Category 1", Glyph = Symbol.Home, Tooltip = "This is category 1" },
                new Category { Name = "Category 2", Glyph = Symbol.Keyboard, Tooltip = "This is category 2" },
                new Category {Name = "Category 3", Glyph = Symbol.Library, Tooltip = "This is category 3" },
                new Category {Name = "Category 4", Glyph = Symbol.Mail, Tooltip = "This is category 3" }
            };
        }

        private void Grid_ManipulationDelta1(object sender, Windows.UI.Xaml.Input.ManipulationDeltaRoutedEventArgs e)
        {
            var grid = sender as Grid;
            grid.Width = grid.ActualWidth + e.Delta.Translation.X;
        }

        private void headerCheck_Click(object sender, RoutedEventArgs e)
        {
            nvSample.AlwaysShowHeader = (sender as CheckBox).IsChecked == true ? true : false;
        }

        private void settingsCheck_Click(object sender, RoutedEventArgs e)
        {
            nvSample.IsSettingsVisible = (sender as CheckBox).IsChecked == true ? true : false;
        }

        private void visibleCheck_Click(object sender, RoutedEventArgs e)
        {
            if ((sender as CheckBox).IsChecked == true)
            {
                nvSample.IsBackButtonVisible = Microsoft.UI.Xaml.Controls.NavigationViewBackButtonVisible.Visible;
            }
            else
            {
                nvSample.IsBackButtonVisible = Microsoft.UI.Xaml.Controls.NavigationViewBackButtonVisible.Collapsed;
            }
        }

        private void enableCheck_Click(object sender, RoutedEventArgs e)
        {
            nvSample.IsBackEnabled = (sender as CheckBox).IsChecked == true ? true : false;
        }

        private void autoSuggestCheck_Click(object sender, RoutedEventArgs e)
        {
            if ((sender as CheckBox).IsChecked == true)
            {
                nvSample.AutoSuggestBox = new AutoSuggestBox() { QueryIcon = new SymbolIcon(Symbol.Find) };
            }
            else
            {
                nvSample.AutoSuggestBox = null;
            }
        }

        private void panemc_Check_Click(object sender, RoutedEventArgs e)
        {
            if ((sender as CheckBox).IsChecked == true)
            {
                PaneHyperlink.Visibility = Visibility.Visible;
            }
            else
            {
                PaneHyperlink.Visibility = Visibility.Collapsed;
            }
        }

        private void paneFooterCheck_Click(object sender, RoutedEventArgs e)
        {
            if ((sender as CheckBox).IsChecked == true)
            {
                FooterStackPanel.Visibility = Visibility.Visible;
            }
            else
            {
                FooterStackPanel.Visibility = Visibility.Collapsed;
            }
        }

        private void panePositionLeft_Click(object sender, RoutedEventArgs e)
        {
            nvSample.PaneDisplayMode = Microsoft.UI.Xaml.Controls.NavigationViewPaneDisplayMode.Left;
            nvSample.IsPaneOpen = true;
            FooterStackPanel.Orientation = Orientation.Vertical;
        }

        private void panePositionTop_Click(object sender, RoutedEventArgs e)
        {
            nvSample.PaneDisplayMode = Microsoft.UI.Xaml.Controls.NavigationViewPaneDisplayMode.Top;
            nvSample.IsPaneOpen = false;
            FooterStackPanel.Orientation = Orientation.Horizontal;
        }

        private void sffCheck_Click(object sender, RoutedEventArgs e)
        {
            if ((sender as CheckBox).IsChecked == true)
            {
                nvSample.SelectionFollowsFocus = Microsoft.UI.Xaml.Controls.NavigationViewSelectionFollowsFocus.Enabled;
            }
            else
            {
                nvSample.SelectionFollowsFocus = Microsoft.UI.Xaml.Controls.NavigationViewSelectionFollowsFocus.Disabled;
            }
        }

        private void suppressselectionCheck_Checked_Click(object sender, RoutedEventArgs e)
        {
            SamplePage2Item.SelectsOnInvoked = (sender as CheckBox).IsChecked == true ? false : true;
        }
    }
}
