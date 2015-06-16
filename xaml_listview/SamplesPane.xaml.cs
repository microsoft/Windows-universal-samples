//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ListViewSample
{
    public sealed partial class SamplesPane : UserControl
    {
        public SamplesPane()
        {
            this.InitializeComponent();
        }
        private void NavigateToOptimized(object sender, RoutedEventArgs e)
        {
            ((Frame)Window.Current.Content).Navigate(typeof(SimpleListViewSample));
        }
        private void NavigateToOptimizedGrid(object sender, RoutedEventArgs e)
        {
            ((Frame)Window.Current.Content).Navigate(typeof(SimpleGridViewSample));
        }
        private void NavigateToMasterDetailSelection(object sender, RoutedEventArgs e)
        {
            ((Frame)Window.Current.Content).Navigate(typeof(MasterDetailSelection));
        }
        private void NavigateToEdgeTappedListView(object sender, RoutedEventArgs e)
        {
            ((Frame)Window.Current.Content).Navigate(typeof(TapOnTheEdgeSample));
        }

        private void NavigateToHome(object sender, RoutedEventArgs e)
        {
            ((Frame)Window.Current.Content).Navigate(typeof(MainPage));
        }
    }
}
