using AppUIBasics.SamplePages;
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

            AddMenuItem(Symbol.Play, "Menu Item1", NavigationMenuItem_Invoked);
            AddMenuItem(Symbol.Save, "Menu Item2", NavigationMenuItem_Invoked_1);
            AddMenuItem(Symbol.Refresh, "Menu Item3", NavigationMenuItem_Invoked_2);
        }

        private void AddMenuItem(Symbol icon, string text, TypedEventHandler<NavigationMenuItem, object> handler)
        {
            var item = new NavigationMenuItem() {
                Icon = new SymbolIcon(icon),
                Text = text };
            item.Invoked += handler;
            nvSample.MenuItems.Add(item);
        }

        private void NavigationMenuItem_Invoked(Windows.UI.Xaml.Controls.NavigationMenuItem sender, object args)
        {
            contentFrame.Navigate(typeof(SamplePage1));
        }

        private void NavigationMenuItem_Invoked_1(Windows.UI.Xaml.Controls.NavigationMenuItem sender, object args)
        {
            contentFrame.Navigate(typeof(SamplePage2));
        }

        private void NavigationMenuItem_Invoked_2(Windows.UI.Xaml.Controls.NavigationMenuItem sender, object args)
        {
            contentFrame.Navigate(typeof(SamplePage3));
        }

        private void NavigationView_Loaded(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            contentFrame.Navigate(typeof(SamplePage1));
        }

        private void rootGrid_SizeChanged(object sender, Windows.UI.Xaml.SizeChangedEventArgs e)
        {
            Example1.Width = e.NewSize.Width;
        }

        private void NavigationView_SettingsInvoked(Windows.UI.Xaml.Controls.NavigationView sender, object args)
        {
            contentFrame.Navigate(typeof(SampleSettingsPage));
        }
    }
}
