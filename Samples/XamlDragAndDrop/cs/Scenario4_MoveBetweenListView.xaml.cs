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
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using System.Collections.ObjectModel;
using Windows.ApplicationModel.DataTransfer;
using System.Linq;
using Windows.Foundation;

namespace DragAndDropSampleManaged
{
    /// <summary>
    /// This sample shows how to drag a single item between listviews and maintain the user's desired location.
    /// </summary>
    public sealed partial class Scenario4_MoveBetweenListView : Page
    {
        private MainPage rootPage;

        ObservableCollection<string> _source;
        ObservableCollection<string> _target;
        string _deletedItem;

        public Scenario4_MoveBetweenListView()
        {
            this.InitializeComponent();

            _source = GetSampleData();
            _target = new ObservableCollection<string>();
            SourceListView.ItemsSource = _source;
            TargetListView.ItemsSource = _target;
        }
        private ObservableCollection<string> GetSampleData()
        {
            return new ObservableCollection<string>
            {
                "My Research Paper",
                "Electricity Bill",
                "My To-do list",
                "TV sales receipt",
                "Water Bill",
                "Grocery List",
                "Superbowl schedule",
                "World Cup E-ticket"
            };
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        /// <summary>
        /// DragItemsStarting is called when the Drag and Drop operation starts
        /// We take advantage of it to set the content of the DataPackage
        /// as well as indicate which operations are supported
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ListView_DragItemsStarting(object sender, DragItemsStartingEventArgs e)
        {
            // Prepare a string with one dragged item per line
            // Set the content of the DataPackage
            e.Data.SetText(e.Items.FirstOrDefault() as string);
            // As we want our Reference list to mutate, we only allow Move
            e.Data.RequestedOperation = DataPackageOperation.Move;
        }

        /// <summary>
        /// DragOver is called when the dragged pointer moves over a UIElement with AllowDrop=True
        /// We need to return an AcceptedOperation != None in either DragOver or DragEnter
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ListView_DragOver(object sender, DragEventArgs e)
        {
            // Our list only accepts text
            e.AcceptedOperation = (e.DataView.Contains(StandardDataFormats.Text)) ? DataPackageOperation.Move : DataPackageOperation.None;
        }

        /// <summary>
        /// We need to return the effective operation from Drop
        /// This is not important for our source ListView, but it might be if the user
        /// drags text from another source
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void ListView_Drop(object sender, DragEventArgs e)
        {
            // This test is in theory not needed as we returned DataPackageOperation.None if
            // the DataPackage did not contained text. However, it is always better if each
            // method is robust by itself
            if (e.DataView.Contains(StandardDataFormats.Text))
            {
                // We need to take a Deferral as we won't be able to confirm the end
                // of the operation synchronously
                var def = e.GetDeferral();
                var text = await e.DataView.GetTextAsync();

                // Get the lists we need to work with.
                var sourceList = (sender == TargetListView ? _source : _target);
                var targetList = (sender == TargetListView ? _target : _source);

                // Remove item from other list
                sourceList.Remove(text);

                // First we need to get the position in the List to drop to
                var listview = sender as ListView;
                var index = -1;

                // Determine which items in the list our pointer is inbetween.
                for (int i = 0; i < listview.Items.Count; i++)
                {
                    var item = listview.ContainerFromIndex(i) as ListViewItem;

                    var p = e.GetPosition(item);

                    if (p.Y - item.ActualHeight < 0)
                    {
                        index = i;
                        break;
                    }
                }

                if (index < 0)
                {
                    // We didn't find a transition point, so we're at the end of the list
                    targetList.Add(text);
                }
                else if (index < listview.Items.Count)
                {
                    // Otherwise, insert at the provided index.
                    targetList.Insert(index, text);
                }

                e.AcceptedOperation = DataPackageOperation.Move;
                def.Complete();
            }
        }
    }
}
