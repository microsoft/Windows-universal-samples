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
using System.Collections;
using Windows.UI.Xaml;

namespace PhoneCall.Helpers
{
    ///////////////////////////////////////////////////
    //   This creates a flyout item that allows binding
    //   to a data context as the default flyout does not
    ///////////////////////////////////////////////////
    public class BindableFlyout : DependencyObject
    {
        #region ItemsSource

        public static IEnumerable GetItemsSource(DependencyObject obj)
        {
            return obj.GetValue(ItemsSourceProperty) as IEnumerable;
        }
        public static void SetItemsSource(DependencyObject obj, IEnumerable value)
        {
            obj.SetValue(ItemsSourceProperty, value);
        }
        public static readonly DependencyProperty ItemsSourceProperty =
            DependencyProperty.RegisterAttached("ItemsSource", typeof(IEnumerable),
            typeof(BindableFlyout), new PropertyMetadata(null, ItemsSourceChanged));
        private static void ItemsSourceChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        { Setup(d as Windows.UI.Xaml.Controls.Flyout); }

        #endregion

        #region ItemTemplate

        public static DataTemplate GetItemTemplate(DependencyObject obj)
        {
            return (DataTemplate)obj.GetValue(ItemTemplateProperty);
        }
        public static void SetItemTemplate(DependencyObject obj, DataTemplate value)
        {
            obj.SetValue(ItemTemplateProperty, value);
        }
        public static readonly DependencyProperty ItemTemplateProperty =
            DependencyProperty.RegisterAttached("ItemTemplate", typeof(DataTemplate),
            typeof(BindableFlyout), new PropertyMetadata(null, ItemsTemplateChanged));
        private static void ItemsTemplateChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        { Setup(d as Windows.UI.Xaml.Controls.Flyout); }

        #endregion

        private static async void Setup(Windows.UI.Xaml.Controls.Flyout m)
        {
            if (Windows.ApplicationModel.DesignMode.DesignModeEnabled)
                return;
            var s = GetItemsSource(m);
            if (s == null)
                return;
            var t = GetItemTemplate(m);
            if (t == null)
                return;
            var c = new Windows.UI.Xaml.Controls.ItemsControl
            {
                ItemsSource = s,
                ItemTemplate = t,
            };
            var n = Windows.UI.Core.CoreDispatcherPriority.Normal;
            Windows.UI.Core.DispatchedHandler h = () => m.Content = c;
            await m.Dispatcher.RunAsync(n, h);
        }
    }
}
