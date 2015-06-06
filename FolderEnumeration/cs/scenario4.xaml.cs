//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Search;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario4 : Page
    {
        public Scenario4()
        {
            this.InitializeComponent();
        }

        private async void GetFilesButton_Click(object sender, RoutedEventArgs e)
        {
            // Reset output.
            OutputPanel.Children.Clear();

            // Ask the user to pick a folder.
            FolderPicker picker = new FolderPicker();
            picker.FileTypeFilter.Add("*");
            picker.ViewMode = PickerViewMode.List;
            picker.SuggestedStartLocation = PickerLocationId.DocumentsLibrary;
            StorageFolder folder = await picker.PickSingleFolderAsync();
            if (folder != null)
            {
                // Query the folder.
                var query = folder.CreateFileQuery();
                var fileList = await query.GetFilesAsync();

                // Display file list with storage provider and availability.
                foreach (StorageFile file in fileList)
                {
                    // Create an entry in the list for the item.
                    var line = file.Name;

                    // Show the item's provider (This PC, OneDrive, Network, or Application Content).
                    line += ": On " + file.Provider.DisplayName;

                    // Show if the item is available (OneDrive items are usually available when
                    // online or when they are marked for "always available offline").
                    line += " (";
                    if (file.IsAvailable)
                    {
                        line += "available";
                    }
                    else
                    {
                        line += "not available";
                    }
                    line += ")";
                    OutputPanel.Children.Add(CreateLineItemTextBlock(line));
                }
            }
        }

        TextBlock CreateLineItemTextBlock(string contents)
        {
            TextBlock textBlock = new TextBlock();
            textBlock.Text = contents;
            textBlock.Style = (Style)Application.Current.Resources["BasicTextStyle"];
            textBlock.TextWrapping = TextWrapping.Wrap;
            return textBlock;
        }
    }
}
