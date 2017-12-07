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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Storage.Provider;

namespace FilePickerContracts
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class CachedFileUpdater_Remote : Page
    {
        public CachedFileUpdater_Remote()
        {
            this.InitializeComponent();
            OverwriteButton.Click += new RoutedEventHandler(OverwriteButton_Click);
            RenameButton.Click += new RoutedEventHandler(RenameButton_Click);
        }

        private async void OutputFileAsync(StorageFile file)
        {
            string fileContent = await FileIO.ReadTextAsync(file);
            OutputFileName.Text = String.Format(@"File Name: {0}", file.Name);
            OutputFileContent.Text = String.Format(@"File Content:{0}{1}", System.Environment.NewLine, fileContent);
        }

        private void UpdateUI()
        {
            OverwriteButton.IsEnabled = false;
            RenameButton.IsEnabled = false;
        }

        private void OverwriteButton_Click(object sender, RoutedEventArgs e)
        {
            FileUpdateRequest fileUpdateRequest = CachedFileUpdaterPage.Current.fileUpdateRequest;
            FileUpdateRequestDeferral fileUpdateRequestDeferral = CachedFileUpdaterPage.Current.fileUpdateRequestDeferral;

            // update the remote version of file...
            // Printing the file content
            this.OutputFileAsync(fileUpdateRequest.File);

            fileUpdateRequest.Status = FileUpdateStatus.Complete;
            fileUpdateRequestDeferral.Complete();

            UpdateUI();
        }

        private async void RenameButton_Click(object sender, RoutedEventArgs e)
        {
            FileUpdateRequest fileUpdateRequest = CachedFileUpdaterPage.Current.fileUpdateRequest;
            FileUpdateRequestDeferral fileUpdateRequestDeferral = CachedFileUpdaterPage.Current.fileUpdateRequestDeferral;

            StorageFile file = await fileUpdateRequest.File.CopyAsync(ApplicationData.Current.LocalFolder, fileUpdateRequest.File.Name, NameCollisionOption.GenerateUniqueName);
            CachedFileUpdater.SetUpdateInformation(file, "CachedFile", ReadActivationMode.NotNeeded, WriteActivationMode.AfterWrite, CachedFileOptions.RequireUpdateOnAccess);
            fileUpdateRequest.UpdateLocalFile(file);

            this.OutputFileAsync(file);

            fileUpdateRequest.Status = FileUpdateStatus.CompleteAndRenamed;
            fileUpdateRequestDeferral.Complete();

            UpdateUI();
        }
    }
}
