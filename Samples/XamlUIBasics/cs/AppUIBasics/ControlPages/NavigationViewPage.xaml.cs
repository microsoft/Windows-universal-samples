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
            //nvSample.SelectedItem = nvSample.MenuItems.OfType<Microsoft.UI.Xaml.Controls.NavigationViewItem>().First();

            //Categories = new ObservableCollection<CategoryBase>()
            //{
            //    new Category { Name = "Category 1", Glyph = Symbol.Home, Tooltip = "This is category 1" },
            //    new Category { Name = "Category 2", Glyph = Symbol.Keyboard, Tooltip = "This is category 2" },
            //    new Category {Name = "Category 3", Glyph = Symbol.Library, Tooltip = "This is category 3" },
            //    new Category {Name = "Category 4", Glyph = Symbol.Mail, Tooltip = "This is category 3" }
            //};
            Categories = new ObservableCollection<CategoryBase>();
            Category firstCategory = new Category { Name = "Category 1", Glyph = Symbol.Home, Tooltip = "This is category 1" };
            Categories.Add(firstCategory);
            Categories.Add(new Category { Name = "Category 2", Glyph = Symbol.Keyboard, Tooltip = "This is category 2" });
            Categories.Add(new Category { Name = "Category 3", Glyph = Symbol.Library, Tooltip = "This is category 3" });
            Categories.Add(new Category { Name = "Category 4", Glyph = Symbol.Mail, Tooltip = "This is category 4" });
            nvSample4.SelectedItem = firstCategory;
        }

        private void NavigationView_Loaded(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            //contentFrame.Navigate(typeof(SamplePage1));
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

        private void rootGrid_SizeChanged(object sender, Windows.UI.Xaml.SizeChangedEventArgs e)
        {
            //Example1.Width = e.NewSize.Width;

            //CameFromGridChange = true;

            //Microsoft.UI.Xaml.Controls.NavigationViewItem ni1 =
            //        new Microsoft.UI.Xaml.Controls.NavigationViewItem();
            //Microsoft.UI.Xaml.Controls.NavigationViewItem ni2 =
            //    new Microsoft.UI.Xaml.Controls.NavigationViewItem();
            //Microsoft.UI.Xaml.Controls.NavigationViewItem ni3 =
            //    new Microsoft.UI.Xaml.Controls.NavigationViewItem();
            //Microsoft.UI.Xaml.Controls.NavigationViewItem ni4 =
            //    new Microsoft.UI.Xaml.Controls.NavigationViewItem();

            //ni1.Content = "Menu Item1";
            //ni1.Tag = "SamplePage1";

            //ni2.Content = "Menu Item2";
            //ni2.Tag = "SamplePage2";

            //ni3.Content = "Menu Item3";
            //ni3.Tag = "SamplePage3";

            //ni4.Content = "Menu Item4";
            //ni4.Tag = "SamplePage3";

            //nvSample2.MenuItems.Clear();
            //nvSample2.MenuItems.Add(ni1);
            //nvSample2.MenuItems.Add(ni2);
            //nvSample2.MenuItems.Add(ni3);
            //nvSample2.MenuItems.Add(ni4);

            //nvSample2.SelectedItem = ni1;

            //if (e.NewSize.Width <= (nvSample2.CompactModeThresholdWidth + 20))
            //{
            //    nvSample2.SelectedItem = nvSample2.MenuItems.OfType<Microsoft.UI.Xaml.Controls.NavigationViewItem>().First();
            //    contentFrame5.Margin = new Thickness(0, 18, 0, 0);
            //    //contentFrame2.Margin = new Thickness(0, 12, 0, 0);
            //} else
            //{
            //    contentFrame5.Margin = new Thickness(12, -30, 0, 0);
            //    //contentFrame2.Margin = new Thickness(0, 0, 0, 0);
            //}
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
            //nvSample.SelectionFollowsFocus = true;
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


        private void NavigationView_SelectionChanged5(Microsoft.UI.Xaml.Controls.NavigationView sender, Microsoft.UI.Xaml.Controls.NavigationViewSelectionChangedEventArgs args)
        {
            //nvSample.SelectionFollowsFocus = true;
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
            //nvSample.SelectionFollowsFocus = true;
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
            //nvSample.SelectionFollowsFocus = true;
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

        private void NavigationView_SelectionChanged4(Microsoft.UI.Xaml.Controls.NavigationView sender, Microsoft.UI.Xaml.Controls.NavigationViewSelectionChangedEventArgs args)
        {
            //nvSample.SelectionFollowsFocus = true;
            if (args.IsSettingsSelected)
            {
                contentFrame4.Navigate(typeof(SampleSettingsPage));
            }
            else
            {
                Debug.WriteLine("Before hitting sample page 1");
                //contentFrame4.Navigate(typeof(SamplePage1));

                var selectedItem = (Category)args.SelectedItem;
                string selectedItemTag = selectedItem.Name;
                sender.Header = "Sample Page " + selectedItemTag.Substring(selectedItemTag.Length - 1);
                string pageName = "AppUIBasics.SamplePages." + "SamplePage1";
                Type pageType = Type.GetType(pageName);
                contentFrame4.Navigate(pageType);
            }
        }


        private void autoSuggestCheck_Checked(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            nvSample.AutoSuggestBox = new AutoSuggestBox() { QueryIcon = new SymbolIcon(Symbol.Find) };
            //XamlContent.ContentTemplate = (DataTemplate)this.Resources["AutoSuggestBoxCode"];
        }

        private void autoSuggestCheck_Unchecked(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            nvSample.AutoSuggestBox = null;
            //XamlContent.ContentTemplate = (DataTemplate)this.Resources["NoAutoSuggestBoxCode"];
        }

        private void hideback_Checked(object sender, RoutedEventArgs e)
        {
            nvSample.IsBackButtonVisible = Microsoft.UI.Xaml.Controls.NavigationViewBackButtonVisible.Collapsed;
            
        }

        private void hideback_Unchecked(object sender, RoutedEventArgs e)
        {
            nvSample.IsBackButtonVisible = Microsoft.UI.Xaml.Controls.NavigationViewBackButtonVisible.Visible;
        }

        private void enableback_Unchecked(object sender, RoutedEventArgs e)
        {
            nvSample.IsBackEnabled = false;
        }

        private void enableback_Checked(object sender, RoutedEventArgs e)
        {
            nvSample.IsBackEnabled = true;
        }

        private void TextBlock_CharacterReceived(UIElement sender, Windows.UI.Xaml.Input.CharacterReceivedRoutedEventArgs args)
        {
            nvSample.PaneTitle = ((TextBox)sender).Text;
        }

        private void settings_checked(object sender, RoutedEventArgs e)
        {
            nvSample.IsSettingsVisible = false;
        }

        private void settings_unchecked(object sender, RoutedEventArgs e)
        {
            nvSample.IsSettingsVisible = true;
        }

        private void TextBlock_LostFocus(object sender, RoutedEventArgs e)
        {
            nvSample.PaneTitle = ((TextBox)sender).Text;
        }

        private void panePositionToggle_Checked(object sender, RoutedEventArgs e)
        {
            nvSample.PaneDisplayMode = Microsoft.UI.Xaml.Controls.NavigationViewPaneDisplayMode.Top;
            FooterStackPanel.Orientation = Orientation.Horizontal;
            //if (paneText.Text == "Welcome")
            //{
            //    PaneHeaderText.Visibility = Visibility.Visible;
            //}
        }

        private void panePositionToggle_Unchecked(object sender, RoutedEventArgs e)
        {
            nvSample.PaneDisplayMode = Microsoft.UI.Xaml.Controls.NavigationViewPaneDisplayMode.Left;
            FooterStackPanel.Orientation = Orientation.Vertical;
            //PaneHeaderText.Visibility = Visibility.Collapsed;
        }

        private void sffCheck_Checked(object sender, RoutedEventArgs e)
        {
            nvSample.SelectionFollowsFocus = Microsoft.UI.Xaml.Controls.NavigationViewSelectionFollowsFocus.Enabled;
        }

        private void sffCheck_Unchecked(object sender, RoutedEventArgs e)
        {
            nvSample.SelectionFollowsFocus = Microsoft.UI.Xaml.Controls.NavigationViewSelectionFollowsFocus.Disabled;
        }

        private void suppressselection_Checked_Unchecked(object sender, RoutedEventArgs e)
        {
            SamplePage2Item.SelectsOnInvoked = true;
        }

        private void suppressselection_Checked(object sender, RoutedEventArgs e)
        {
            SamplePage2Item.SelectsOnInvoked = false;
        }

        private void panemc_Check_Checked(object sender, RoutedEventArgs e)
        {
            PaneHyperlink.Visibility = Visibility.Visible;
        }

        private void panemc_Check_Unchecked(object sender, RoutedEventArgs e)
        {
            PaneHyperlink.Visibility = Visibility.Collapsed;
        }

        private void paneFooterCheck_Checked(object sender, RoutedEventArgs e)
        {
            FooterStackPanel.Visibility = Visibility.Visible;
        }

        private void paneFooterCheck_Unchecked(object sender, RoutedEventArgs e)
        {
            FooterStackPanel.Visibility = Visibility.Collapsed;
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
    }
}
