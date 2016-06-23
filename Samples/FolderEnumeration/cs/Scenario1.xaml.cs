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
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1 : Page
    {
        public Scenario1()
        {
            this.InitializeComponent();
        }

        private async void GetFilesAndFoldersButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFolder picturesFolder = await KnownFolders.GetFolderForUserAsync(null /* current user */, KnownFolderId.PicturesLibrary);

            IReadOnlyList<StorageFile> fileList = await picturesFolder.GetFilesAsync();
            IReadOnlyList<StorageFolder> folderList = await picturesFolder.GetFoldersAsync();

            var count = fileList.Count + folderList.Count;
            StringBuilder outputText = new StringBuilder(picturesFolder.Name + " (" + count + ")\n\n");

            foreach (StorageFolder folder in folderList)
            {
                outputText.AppendLine("    " + folder.DisplayName + "\\");
            }

            foreach (StorageFile file in fileList)
            {
                outputText.AppendLine("    " + file.Name);
            }

            OutputTextBlock.Text = outputText.ToString();
        }
    }
}
