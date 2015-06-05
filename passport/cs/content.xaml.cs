using SDKTemplate;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Content : Page
    {
        private MainPage rootPage;

        public Content()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private void Button_Restart_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(UserSelect));
        }
    }
}
