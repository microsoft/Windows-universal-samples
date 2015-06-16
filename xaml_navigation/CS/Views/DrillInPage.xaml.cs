using System;
using System.Collections.Generic;
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

namespace NavigationMenuSample.Views
{
    public sealed partial class DrillInPage : Page
    {
        public DrillInPage()
        {
            this.InitializeComponent();
        }

        private void ListView_ItemClick(object sender, ItemClickEventArgs e)
        {
            this.Frame.Navigate(
                typeof(BasicSubPage),
                e.ClickedItem,
                new Windows.UI.Xaml.Media.Animation.DrillInNavigationTransitionInfo());
        }
    }
}
