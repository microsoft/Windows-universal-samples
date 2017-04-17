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

using SDKTemplate;
using System;
using System.Collections.Generic;
using System.Linq;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    public sealed partial class Scenario3 : Page
    {
        private MainPage rootPage = MainPage.Current;
        public List<SampleDataModel> AllItems = SampleDataModel.GetSampleData();
        private MenuFlyout sharedFlyout;

        public Scenario3()
        {
            this.InitializeComponent();
            sharedFlyout = (MenuFlyout)Resources["SampleContextMenu"];
        }

        private void ListView_ContextRequested(UIElement sender, ContextRequestedEventArgs e)
        {
            // Walk up the tree to find the ListViewItem.
            // There may not be one if the click wasn't on an item.
            var requestedElement = (FrameworkElement)e.OriginalSource;
            while ((requestedElement != sender) && !(requestedElement is ListViewItem))
            {
                requestedElement = (FrameworkElement)VisualTreeHelper.GetParent(requestedElement);
            }
            if (requestedElement != sender)
            {
                // The context menu request was indeed for a ListViewItem.
                var model = (SampleDataModel)ItemListView.ItemFromContainer(requestedElement);
                Point point;
                if (e.TryGetPosition(requestedElement, out point))
                {
                    rootPage.NotifyUser($"Showing flyout for {model.Title} at {point}", NotifyType.StatusMessage);
                    sharedFlyout.ShowAt(requestedElement, point);
                }
                else
                {
                    // Not invoked via pointer, so let XAML choose a default location.
                    rootPage.NotifyUser($"Showing flyout for {model.Title} at default location", NotifyType.StatusMessage);
                    sharedFlyout.ShowAt(requestedElement);
                }

                e.Handled = true;
            }
        }

        private void ListView_ContextCanceled(UIElement sender, RoutedEventArgs args)
        {
            sharedFlyout.Hide();
        }

        private SampleDataModel GetDataModelForCurrentListViewFlyout()
        {
            // Obtain the ListViewItem for which the user requested a context menu.
            var listViewItem = sharedFlyout.Target;

            // Get the data model for the ListViewItem.
            return (SampleDataModel)ItemListView.ItemFromContainer(listViewItem);
        }

        private void OpenMenuItem_Click(object sender, RoutedEventArgs e)
        {
            SampleDataModel model = GetDataModelForCurrentListViewFlyout();
            rootPage.NotifyUser($"Item: {model.Title}, Action: Open", NotifyType.StatusMessage);
        }

        private void PrintMenuItem_Click(object sender, RoutedEventArgs e)
        {
            SampleDataModel model = GetDataModelForCurrentListViewFlyout();
            rootPage.NotifyUser($"Item: {model.Title}, Action: Print", NotifyType.StatusMessage);
        }

        private void DeleteMenuItem_Click(object sender, RoutedEventArgs e)
        {
            SampleDataModel model = GetDataModelForCurrentListViewFlyout();
            rootPage.NotifyUser($"Item: {model.Title}, Action: Delete", NotifyType.StatusMessage);
        }
    }
}
