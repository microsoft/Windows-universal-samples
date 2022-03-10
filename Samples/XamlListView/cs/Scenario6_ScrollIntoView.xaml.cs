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

using System;
using System.Collections.ObjectModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario6_ScrollIntoView : Page
    {
        private ObservableCollection<Item> _items = Item.GetItems(1000);

        public Scenario6_ScrollIntoView()
        {
            InitializeComponent();
        }

        private void Scroll_Click(object sender, RoutedEventArgs e)
        {
            int numVal;
            if (Int32.TryParse(scrollId.Text, out numVal) && numVal >= 0 && numVal < ItemsListView.Items.Count)
            {
                var item = (ComboBoxItem)ScrollAlignment.SelectedItem;
                var Alignment = (ScrollIntoViewAlignment)item.Tag;
                ItemsListView.ScrollIntoView(ItemsListView.Items[numVal], Alignment);
                scrollId.BorderBrush = ValidFormatBrush;
            }
            else
            {
                scrollId.BorderBrush = InvalidFormatBrush;
            }
        }
    }
}
