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
    public sealed partial class Scenario3 : Page
    {
        StorageLibrary picturesLibrary;

        public Scenario3()
        {
            this.InitializeComponent();
            this.GetPicturesLibrary();
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
        /// Requests the user's permission to remove the selected location from the Pictures library.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void RemoveFolderButton_Click(object sender, RoutedEventArgs e)
        {
            var folderToRemove = (StorageFolder)FoldersComboBox.SelectedItem;
            if (await picturesLibrary.RequestRemoveFolderAsync(folderToRemove))
            {
                OutputTextBlock.Text = folderToRemove.DisplayName + " was removed from the Pictures library.";
            }
            else
            {
                OutputTextBlock.Text = "Operation canceled.";
            }
        }

        /// <summary>
        /// Gets the Pictures library and sets up the FoldersComboBox to list its folders.
        /// </summary>
        private async void GetPicturesLibrary()
        {
            picturesLibrary = await StorageLibrary.GetLibraryAsync(KnownLibraryId.Pictures);

            // Bind the ComboBox to the list of folders currently in the library
            FoldersComboBox.ItemsSource = picturesLibrary.Folders;

            // Update the states of our controls when the list of folders changes
            picturesLibrary.DefinitionChanged += async (StorageLibrary sender, object e) =>
                {
                    await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        UpdateControls();
                    });
                };

            UpdateControls();
        }

        /// <summary>
        // Fills the ComboBox control with the folders that make up the Pictures library.
        /// </summary>
        private void FillComboBox()
        {
            FoldersComboBox.ItemsSource = null;
            FoldersComboBox.ItemsSource = picturesLibrary.Folders;
        }

        /// <summary>
        /// Updates the Visibility and IsEnabled properties of UI controls that depend upon the Pictures library
        /// having at least one folder in its Folders list.
        /// </summary>
        private void UpdateControls()
        {
            FillComboBox();
            bool libraryHasFolders = (picturesLibrary.Folders.Count > 0);
            FoldersComboBox.SelectedIndex = libraryHasFolders ? 0 : -1;
            FoldersComboBox.Visibility = libraryHasFolders ? Visibility.Visible : Visibility.Collapsed;
            LibraryEmptyTextBlock.Visibility = libraryHasFolders ? Visibility.Collapsed : Visibility.Visible;
            RemoveFolderButton.IsEnabled = libraryHasFolders;
        }
    }
}
