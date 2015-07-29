using OneHandedUse_TailoredMultipleViews.UserControls;
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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace OneHandedUse_TailoredMultipleViews
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
            this.Loaded += MainPage_Loaded;
        }

        private void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            list.SelectedIndex = 0;
        }
        private void SplitViewButton_Click(object sender, RoutedEventArgs e)
        {
            mainSplitview.IsPaneOpen = !mainSplitview.IsPaneOpen;
        }

        private void list_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            switch (list.SelectedIndex)
            {
                case 0:
                    mainContentPC.Content = new Collections();
                    break;
                case 1:
                    mainContentPC.Content = new Albums();
                    break;
                case 2:
                    mainContentPC.Content = new Settings();
                    break;
            }
        }
    }
}
