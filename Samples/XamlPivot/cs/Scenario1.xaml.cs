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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace PivotCS
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage;
        public List<SampleDataModel> NewItems = null;
        public List<SampleDataModel> FlaggedItems = null;
        public List<SampleDataModel> AllItems = null;

        public Scenario1()
        {
            this.InitializeComponent();

            NewItems = SampleDataModel.GetSampleData().Where(x => x.IsNew).ToList();
            FlaggedItems = SampleDataModel.GetSampleData().Where(x => x.IsFlagged).ToList();
            AllItems = SampleDataModel.GetSampleData().ToList();

            this.DataContext = this;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }


        private void ListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            SampleDataModel SelectedItem = null;

            if (e.AddedItems.Count >= 1)
            {
                SelectedItem = e.AddedItems[0] as SampleDataModel;
                (sender as ListView).SelectedItem = null;
                ShowItem(SelectedItem);
            }

        }
        private void ShowItem(SampleDataModel model)
        {
            var MyDialog = new ContentDialog();

            if (model != null)
            {
                var MyImage = new Image();
                MyImage.Source = new BitmapImage(new Uri(model.ImagePath));
                MyImage.HorizontalAlignment = HorizontalAlignment.Stretch;
                MyImage.VerticalAlignment = VerticalAlignment.Stretch;
                MyImage.Stretch = Windows.UI.Xaml.Media.Stretch.UniformToFill;
                MyDialog.Title = String.Format("You have selected {0}", model.ToString());
                MyDialog.Content = MyImage;
            }
            else
            {
                MyDialog.Title = "No item found";

            }

            MyDialog.PrimaryButtonText = "OK";

            Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () => MyDialog.ShowAsync());
        }
        private void Scenario2_Click(Windows.UI.Xaml.Documents.Hyperlink sender, Windows.UI.Xaml.Documents.HyperlinkClickEventArgs args)
        {
            this.Frame.Navigate(typeof(Scenario2));
        }
        private void Scenario3_Click(Windows.UI.Xaml.Documents.Hyperlink sender, Windows.UI.Xaml.Documents.HyperlinkClickEventArgs args)
        {
            this.Frame.Navigate(typeof(Scenario3));
        }
    }
}
