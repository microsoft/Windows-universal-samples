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
    public sealed partial class CachedFileUpdater_Local : Page
    {
        public CachedFileUpdater_Local()
        {
            this.InitializeComponent();
            FileIsCurrentButton.Click += new RoutedEventHandler(FileIsCurrentButton_Click);
            ProvideUpdatedVersionButton.Click += new RoutedEventHandler(ProvideUpdatedVersionButton_Click);
        }

        private async void OutputFileAsync(StorageFile file)
        {
            string fileContent = await FileIO.ReadTextAsync(file);
            OutputFileName.Text = String.Format(@"Received file: {0}", file.Name);
            OutputFileContent.Text = String.Format(@"File content:{0}{1}", System.Environment.NewLine, fileContent);
        }

        private void UpdateUI(UIStatus uiStatus)
        {
            if (uiStatus == UIStatus.Complete)
            {
                FileIsCurrentButton.IsEnabled = false;
                ProvideUpdatedVersionButton.IsEnabled = false;
            }
        }

        private void FileIsCurrentButton_Click(object sender, RoutedEventArgs e)
        {
            FileUpdateRequest fileUpdateRequest = CachedFileUpdaterPage.Current.fileUpdateRequest;
            FileUpdateRequestDeferral fileUpdateRequestDeferral = CachedFileUpdaterPage.Current.fileUpdateRequestDeferral;

            OutputFileAsync(fileUpdateRequest.File);

            fileUpdateRequest.Status = FileUpdateStatus.Complete;
            fileUpdateRequestDeferral.Complete();

            UpdateUI(CachedFileUpdaterPage.Current.cachedFileUpdaterUI.UIStatus);
        }

        private async void ProvideUpdatedVersionButton_Click(object sender, RoutedEventArgs e)
        {
            FileUpdateRequest fileUpdateRequest = CachedFileUpdaterPage.Current.fileUpdateRequest;
            FileUpdateRequestDeferral fileUpdateRequestDeferral = CachedFileUpdaterPage.Current.fileUpdateRequestDeferral;

            await FileIO.AppendTextAsync(fileUpdateRequest.File, String.Format(@"{0}New content added @ {1}", System.Environment.NewLine, DateTime.Now.ToString()));
            OutputFileAsync(fileUpdateRequest.File);

            fileUpdateRequest.Status = FileUpdateStatus.Complete;
            fileUpdateRequestDeferral.Complete();

            UpdateUI(CachedFileUpdaterPage.Current.cachedFileUpdaterUI.UIStatus);
        }
    }
}
