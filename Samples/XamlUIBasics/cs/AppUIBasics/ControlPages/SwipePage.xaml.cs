//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using System.Collections.ObjectModel;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace AppUIBasics.ControlPages
{
    public sealed partial class SwipeControlPage : Page
    {
        public SwipeControlPage()
        {
            this.InitializeComponent();
            var source = @"AcrylicBrush ColorPicker NavigationView ParallaxView PersonPicture RatingControl RevealBrush".Split(' ');
            foreach (var item in source)
                items.Add(item);
            lv.ItemsSource = items;
        }

        ObservableCollection<object> items = new ObservableCollection<object>();

        private void DeleteOne_ItemInvoked(SwipeItem sender, SwipeItemInvokedEventArgs args)
        {
            args.SwipeControl.Visibility = Visibility.Collapsed;
        }

        private void DeleteItem_ItemInvoked(SwipeItem sender, SwipeItemInvokedEventArgs args)
        {
            int i = lv.Items.IndexOf(args.SwipeControl.DataContext);
            items.RemoveAt(1);
        }

        private void Accept_ItemInvoked(SwipeItem sender, SwipeItemInvokedEventArgs args)
        {
            args.SwipeControl.Background = new SolidColorBrush(Colors.Cyan);
        }

        private void Flag_ItemInvoked(SwipeItem sender, SwipeItemInvokedEventArgs args)
        {
            args.SwipeControl.Background = new SolidColorBrush(Colors.Green);
        }
    }
}