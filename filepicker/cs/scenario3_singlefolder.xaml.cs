//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using SDKTemplate;

using System;
using Windows.Storage;
using Windows.Storage.AccessCache;
using Windows.Storage.Pickers;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace FilePicker
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario3 : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario3()
        {
            this.InitializeComponent();
            PickFolderButton.Click += new RoutedEventHandler(PickFolderButton_Click);
        }

        private async void PickFolderButton_Click(object sender, RoutedEventArgs e)
        {
            // Clear previous returned folder name, if it exists, between iterations of this scenario
            OutputTextBlock.Text = "";

            FolderPicker folderPicker = new FolderPicker();
            folderPicker.SuggestedStartLocation = PickerLocationId.Desktop;
            folderPicker.FileTypeFilter.Add(".docx");
            folderPicker.FileTypeFilter.Add(".xlsx");
            folderPicker.FileTypeFilter.Add(".pptx");
            StorageFolder folder = await folderPicker.PickSingleFolderAsync();
            if (folder != null)
            {
                // Application now has read/write access to all contents in the picked folder (including other sub-folder contents)
                StorageApplicationPermissions.FutureAccessList.AddOrReplace("PickedFolderToken", folder);
                OutputTextBlock.Text = "Picked folder: " + folder.Name;
            }
            else
            {
                OutputTextBlock.Text = "Operation cancelled.";
            }
        }
    }
}
