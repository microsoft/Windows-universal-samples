using AppUIBasics.SamplePages;
using System;
using Windows.Foundation;
using Windows.Foundation.Metadata;
using Windows.UI.Xaml;
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
        }

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

        private void autoSuggestCheck_Checked(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            nvSample.AutoSuggestBox = new AutoSuggestBox() { QueryIcon= new SymbolIcon(Symbol.Find)};
            XamlContent.ContentTemplate = (DataTemplate)this.Resources["AutoSuggestBoxCode"];
        }

        private void autoSuggestCheck_Unchecked(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            nvSample.AutoSuggestBox = null;
            XamlContent.ContentTemplate = (DataTemplate)this.Resources["NoAutoSuggestBoxCode"];
        }

        private void hideback_Checked(object sender, RoutedEventArgs e)
        {
            if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 6))
            {
                nvSample.IsBackButtonVisible = NavigationViewBackButtonVisible.Collapsed;
            }
        }

        private void hideback_Unchecked(object sender, RoutedEventArgs e)
        {
            if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 6))
            {
                nvSample.IsBackButtonVisible = NavigationViewBackButtonVisible.Visible;
            }
        }

        private void enableback_Unchecked(object sender, RoutedEventArgs e)
        {
            if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 6))
            {
                nvSample.IsBackEnabled = false;
            }
        }

        private void enableback_Checked(object sender, RoutedEventArgs e)
        {
            if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 6))
            {
                nvSample.IsBackEnabled = true;
            }
        }

        private void TextBlock_CharacterReceived(UIElement sender, Windows.UI.Xaml.Input.CharacterReceivedRoutedEventArgs args)
        {
            if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 6))
            {
                nvSample.PaneTitle = ((TextBox)sender).Text;
            }
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
            if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 6))
            {
                nvSample.PaneTitle = ((TextBox)sender).Text;
            }
        }
    }
}
