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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace App1.Views
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Shell : Page
    {
        public Shell()
        {
            this.InitializeComponent();
            this.HamburguerButton.Visibility = this.SplitView.DisplayMode == SplitViewDisplayMode.Overlay? Visibility.Visible : Visibility.Collapsed;
        }

        private void HamburgerRadioButton_Click(object sender, RoutedEventArgs e)
        {
            this.SplitView.IsPaneOpen = !this.SplitView.IsPaneOpen;


        }

        private void HomeRadioButton_Click(object sender, RoutedEventArgs e)
        {
            var frame = this.DataContext as Frame;
            Page page = frame?.Content as Page;
            if (page?.GetType() != typeof(MainPage))
            {
                frame.Navigate(typeof(MainPage));
            }
        }

        private void FriendsRadioButton_Click(object sender, RoutedEventArgs e)
        {
            var frame = this.DataContext as Frame;
            Page page = frame?.Content as Page;
            if (page?.GetType() != typeof(FriendsPage))
            {
                frame.Navigate(typeof(FriendsPage));
            }
        }

        private void BackRadionButton_Click(object sender, RoutedEventArgs e)
        {
            var frame = this.DataContext as Frame;
            if (frame?.CanGoBack == true)
            {
                frame.GoBack();
            }

        }

        private void HamburguerButton_Click(object sender, RoutedEventArgs e)
        {
            this.SplitView.IsPaneOpen = !this.SplitView.IsPaneOpen;
            this.HamburguerButton.Visibility = Visibility.Collapsed;
        }

        private void SplitView_PaneClosed(SplitView sender, object args)
        {
            if (this.SplitView.DisplayMode == SplitViewDisplayMode.Overlay)
                this.HamburguerButton.Visibility = Visibility.Visible;
        }

    }
}
