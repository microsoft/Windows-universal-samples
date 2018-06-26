using AppUIBasics.SamplePages;
using System;
using Windows.Foundation;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class NavigationViewPage : Page
    {
        public NavigationViewPage()
        {
            this.InitializeComponent();

            //AddMenuItem(Symbol.Play, "Menu Item1", typeof(SamplePage1));
            //AddMenuItem(Symbol.Save, "Menu Item2", typeof(SamplePage2));
            //AddMenuItem(Symbol.Refresh, "Menu Item3", typeof(SamplePage3));
        }

        //private void AddMenuItem(Symbol icon, string text, Type pageType)
        //{
        //    var item = new NavigationViewItem()
        //    {
        //        Icon = new SymbolIcon(icon),
        //        Content = text,
        //        Tag = pageType
        //    };
        //    nvSample.MenuItems.Add(item);
        //}

        private void NavigationView_Loaded(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            contentFrame.Navigate(typeof(SamplePage1));
        }

        private void rootGrid_SizeChanged(object sender, Windows.UI.Xaml.SizeChangedEventArgs e)
        {
            Example1.Width = e.NewSize.Width;
        }

        private void NavigationView_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
        {
            if (args.IsSettingsSelected)
            {
                contentFrame.Navigate(typeof(SampleSettingsPage));
            }
            else
            {
                var selectedItem = (NavigationViewItem)args.SelectedItem;
                string pageName = "AppUIBasics.SamplePages." + ((string) selectedItem.Tag);
                Type pageType = Type.GetType(pageName);
                contentFrame.Navigate(pageType);
            }
        }
    }
}
