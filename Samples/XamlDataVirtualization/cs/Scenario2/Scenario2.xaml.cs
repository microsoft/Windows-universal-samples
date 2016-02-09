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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using Windows.Storage;

namespace DataVirtualizationSample
{
    public sealed partial class Scenario2 : Page
    {
        public Scenario2()
        {
            this.InitializeComponent();
            Grid1.ContainerContentChanging += Grid1_ContainerContentChanging;
            initdata();
        }

        private void Grid1_ContainerContentChanging(ListViewBase sender, ContainerContentChangingEventArgs args)
        {
            if (!args.InRecycleQueue)
            {
                FrameworkElement ctr = (FrameworkElement)args.ItemContainer.ContentTemplateRoot;
                if (ctr != null)
                {
                    TextBlock t = (TextBlock)ctr.FindName("idx");
                    t.Text = args.ItemIndex.ToString();
                }
            }
        }

        async void initdata()
        {
            StorageLibrary pictures = await StorageLibrary.GetLibraryAsync(KnownLibraryId.Pictures);
            string path = pictures.SaveFolder.Path;

            FileDataSource2 ds = await FileDataSource2.GetDataSoure(path);
            if (ds.Count > 0)
            {
                Grid1.ItemsSource = ds;
            }
            else
            {
                MainPage.Current.NotifyUser("Error: The pictures folder doesn't contain any files", NotifyType.ErrorMessage);
            }
        }

        private void ForceReset_Click(object sender, RoutedEventArgs e)
        {
            FileDataSource2 ds = Grid1.ItemsSource as FileDataSource2;
            if (ds != null) ds.ResetCollection();
        }
    }
}
