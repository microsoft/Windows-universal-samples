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

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class CompactSizingPage : Page
    {

        ResourceDictionary compact = new ResourceDictionary();
        Uri source = new Uri("ms-appx:///ControlPages/CompactSizing.xaml");

        public CompactSizingPage()
        {
            this.InitializeComponent();
        }

        private void Standard_Checked(object sender, RoutedEventArgs e)
        {
            //compact.Source = source;
            //CompactGrid.Resources.MergedDictionaries.Remove(compact);


        }

        private void Compact_Checked(object sender, RoutedEventArgs e)
        {            
            //compact.Source = source;
            //CompactGrid.Resources.MergedDictionaries.Insert(0, compact);

            Frame.Navigate(typeof(StandardSizingPage));
        }
    }
}
