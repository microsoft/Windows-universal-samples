//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
//using Microsoft.UI.Preview;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class SwipePage : Page
    {
#if false
        public SwipePage()
        {
            this.InitializeComponent();

            lv.ItemsSource = @"AcrylicBrush ColorPicker NavigationView ParallaxView PersonPicture PullToRefresh RatingsControl RevealBrush TreeView".Split(' ');
        }

        ObservableCollection<object> items = new ObservableCollection<object>();

        private void SwipeContent_ItemInvoked(SwipeItem sender, SwipeItemInvokedEventArgs args)
        {
            //if (sender.Text == "Delete")
            //{
            //    items.RemoveAt(1);
            //}
            //else if (sender.Text == "Like")
            //{
            //    ListViewItem lvi = lv.ContainerFromIndex(0) as ListViewItem;

            //    //(lvi.ContentTemplateRoot as SwipeContainer).Background = new SolidColorBrush(sender.Background);
            //    //(lvi.ContentTemplateRoot as SwipeContainer).Reset();
            //}
            //else if (sender.Text == "Dislike")
            //{
            //    ListViewItem lvi = lv.ContainerFromIndex(0) as ListViewItem;

            //    //(lvi.ContentTemplateRoot as SwipeContainer).Background = new SolidColorBrush(sender.Background);
            //    //(lvi.ContentTemplateRoot as SwipeContainer).Reset();
            //}
            //else
            //{
            //    ListViewItem lvi = lv.ContainerFromIndex(0) as ListViewItem;
            //    (lvi.ContentTemplateRoot as SwipeContainer).Reset();
            //}
        }
#else
        public SwipePage()
        {
            this.InitializeComponent();
        }
#endif
    }
}
