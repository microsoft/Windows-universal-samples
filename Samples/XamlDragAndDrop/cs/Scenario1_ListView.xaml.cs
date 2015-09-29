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
using System.Text;
using Windows.ApplicationModel.DataTransfer;
using System.Diagnostics;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace DragAndDropSampleManaged
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_ListView : Page
    {
        private MainPage rootPage;

        ObservableCollection<string> _reference;
        ObservableCollection<string> _selection;
        string _deletedItem;

        public Scenario1_ListView()
        {
            this.InitializeComponent();

            _reference = GetSampleData();
            _selection = new ObservableCollection<string>();
            SourceListView.ItemsSource = _reference;
            TargetListView.ItemsSource = _selection;

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
        private void SourceListView_DragItemsStarting(object sender, DragItemsStartingEventArgs e)
        {
            // Prepare a string with one dragged item per line
            var items = new StringBuilder();
            foreach (var item in e.Items)
            {
                if (items.Length > 0) items.AppendLine();
                items.Append(item as string);
            }
            // Set the content of the DataPackage
            e.Data.SetText(items.ToString());
            // As we want our Reference list to say intact, we only allow Copy
            e.Data.RequestedOperation = DataPackageOperation.Copy;
        }

        /// <summary>
        /// DragOver is called when the dragged pointer moves over a UIElement with AllowDrop=True
        /// We need to return an AcceptedOperation != None in either DragOver or DragEnter
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void TargetListView_DragOver(object sender, DragEventArgs e)
        {
            // Our list only accepts text
            e.AcceptedOperation = (e.DataView.Contains(StandardDataFormats.Text)) ? DataPackageOperation.Copy : DataPackageOperation.None;
        }

        /// <summary>
        /// We need to return the effective operation from Drop
        /// This is not important for our source ListView, but it might be if the user
        /// drags text from another source
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void TargetListView_Drop(object sender, DragEventArgs e)
        {
            // This test is in theory not needed as we returned DataPackageOperation.None if
            // the DataPackage did not contained text. However, it is always better if each
            // method is robust by itself
            if (e.DataView.Contains(StandardDataFormats.Text))
            {
                // We need to take a Deferral as we won't be able to confirm the end
                // of the operation synchronously
                var def = e.GetDeferral();
                var s = await e.DataView.GetTextAsync();
                var items = s.Split('\n');
                foreach (var item in items)
                {
                    _selection.Add(item);
                }
                e.AcceptedOperation = DataPackageOperation.Copy;
                def.Complete();
            }
        }

        /// <summary>
        /// DragtemsStarting is called for D&D and reorder as the framework does not
        /// know wherer the user will drop the items. Reorder means that the target
        /// and the source ListView are the same.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void TargetListView_DragItemsStarting(object sender, DragItemsStartingEventArgs e)
        {
            // The ListView is declared with selection mode set to Single.
            // But we want the code to be robust
            if (e.Items.Count == 1)
            {
                e.Data.SetText(e.Items[0] as string);
                // Reorder or move to trash are always a move
                e.Data.RequestedOperation = DataPackageOperation.Move;
                _deletedItem = null;
            }
        }

        /// <summary>
        /// Called at the end of the operation, whether it was a reorder or move to trash
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private void TargetListView_DragItemsCompleted(ListViewBase sender, DragItemsCompletedEventArgs args)
        {
            // args.DropResult is always Move and therefore we have to rely on _deletedItem to distinguish
            // between reorder and move to trash
            // Another solution would be to listen for events in the ObservableCollection
            if (_deletedItem != null)
            {
                _selection.Remove(_deletedItem);
                _deletedItem = null;
            }
        }

        /// <summary>
        /// Entering the Trash icon
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void TargetTextBlock_DragEnter(object sender, DragEventArgs e)
        {
            // Trash only accepts text
            e.AcceptedOperation = (e.DataView.Contains(StandardDataFormats.Text) ? DataPackageOperation.Move : DataPackageOperation.None);
            // We don't want to show the Move icon
            e.DragUIOverride.IsGlyphVisible = false;
            e.DragUIOverride.Caption = "Drop item here to remove it from selection";
        }


        /// <summary>
        /// Drop on the Trash
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void TargetTextBlock_Drop(object sender, DragEventArgs e)
        {
            if (e.DataView.Contains(StandardDataFormats.Text))
            {
                // We need to take the deferral as the source will read _deletedItem which
                // we cannot set synchronously
                var def = e.GetDeferral();
                _deletedItem = await e.DataView.GetTextAsync();
                e.AcceptedOperation = DataPackageOperation.Move;
                def.Complete();
            }
        }
    }
}
