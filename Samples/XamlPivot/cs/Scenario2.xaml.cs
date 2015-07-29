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
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace PivotCS
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2 : Page
    {
        public ObservableCollection<SampleDataModel> Suggestions { get; private set; }
        public List<SampleDataModel> NewItems = null;
        public List<SampleDataModel> FlaggedItems = null;
        public List<SampleDataModel> AllItems = null;

        public Scenario2()
        {
            this.InitializeComponent();
            NewItems = SampleDataModel.GetSampleData().Where(x => x.IsNew).ToList();
            FlaggedItems = SampleDataModel.GetSampleData().Where(x => x.IsFlagged).ToList();
            AllItems = SampleDataModel.GetSampleData().ToList();
            this.Suggestions = new ObservableCollection<SampleDataModel>();

            this.DataContext = this;
        }


        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            if (this.Frame.CanGoBack)
                this.Frame.GoBack();
        }

        private void AutoSuggestBox_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
        {
            // TODO: not being called from flyout
            if (args.Reason == AutoSuggestionBoxTextChangeReason.UserInput)
            {
                Suggestions.Clear();
                foreach (var Item in AllItems)
                {
                    if (Item.Title.IndexOf(sender.Text, StringComparison.OrdinalIgnoreCase) >= 0)
                    {
                        Suggestions.Add(Item);
                    }
                }
            }
        }


        private void AutoSuggestBox_QuerySubmitted(AutoSuggestBox sender, AutoSuggestBoxQuerySubmittedEventArgs args)
        {
            SampleDataModel FoundItem = null;

            if (args.ChosenSuggestion != null && args.ChosenSuggestion is SampleDataModel)
            {
                FoundItem = args.ChosenSuggestion as SampleDataModel;
            }
            else if (String.IsNullOrEmpty(args.QueryText) == false)
            {
                foreach (var Item in AllItems)
                {
                    if (Item.Title.Equals(args.QueryText, StringComparison.OrdinalIgnoreCase))
                    {
                        FoundItem = Item;
                        break;
                    }
                }
            }

            ShowItem(FoundItem);
        }

        private void AutoSuggestBox_SuggestionChosen(AutoSuggestBox sender, AutoSuggestBoxSuggestionChosenEventArgs args)
        {
            ShowItem(args.SelectedItem as SampleDataModel);
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
    }
}
