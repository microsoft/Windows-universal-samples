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

namespace SDKTemplate
{
    public sealed partial class Scenario2 : Page
    {
        private MainPage rootPage = MainPage.Current;
        public List<SampleDataModel> AllItems = SampleDataModel.GetSampleData();

        public Scenario2()
        {
            this.InitializeComponent();
        }

        private SampleDataModel GetDataModelForCurrentListViewFlyout()
        {
            // Obtain the ListViewItem for which the user requested a context menu.
            var listViewItem = SharedFlyout.Target;

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
