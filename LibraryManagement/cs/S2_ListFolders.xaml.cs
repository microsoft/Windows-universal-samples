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
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2 : Page
    {
        StorageLibrary picturesLibrary;

        public Scenario2()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }

        /// <summary>
        /// Obtains the list of folders that make up the Pictures library and binds the FoldersListView
        /// to this list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void ListFoldersButton_Click(object sender, RoutedEventArgs e)
        {
            ListFoldersButton.IsEnabled = false;
            picturesLibrary = await StorageLibrary.GetLibraryAsync(KnownLibraryId.Pictures);

            // Bind the FoldersListView to the list of folders that make up the library
            FoldersListView.ItemsSource = picturesLibrary.Folders;

            // Register for the DefinitionChanged event to be notified if other apps modify the library
            picturesLibrary.DefinitionChanged += async (StorageLibrary innerSender, object innerEvent) =>
                {
                    await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                    {
                        UpdateHeaderText();
                    });
                };
            UpdateHeaderText();
        }

        /// <summary>
        /// Updates the FoldersListHeaderTextBlock with the count of folders in the Pictures library.
        /// </summary>
        private void UpdateHeaderText()
        {
            FoldersListHeaderTextBlock.Text = "Pictures library (" + picturesLibrary.Folders.Count + " folders)";
        }
    }
}
