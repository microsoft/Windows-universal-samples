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
using Windows.ApplicationModel.DataTransfer;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.Xaml;

namespace SDKTemplate
{
    public sealed partial class ShareFiles
    {
        private IReadOnlyList<StorageFile> storageItems;

        public ShareFiles()
        {
            this.InitializeComponent();
        }

        protected override bool GetShareContent(DataRequest request)
        {
            bool succeeded = false;

            if (storageItems != null)
            {
                DataPackage requestData = request.Data;
                requestData.Properties.Title = TitleInputBox.Text;
                requestData.Properties.Description = DescriptionInputBox.Text; // The description is optional.
                requestData.Properties.ContentSourceApplicationLink = ApplicationLink;
                requestData.SetStorageItems(this.storageItems);
                succeeded = true;
            }
            else
            {
                request.FailWithDisplayText("Select the files you would like to share and try again.");
            }
            return succeeded;
        }

        private async void SelectFilesButton_Click(object sender, RoutedEventArgs e)
        {
            FileOpenPicker filePicker = new FileOpenPicker
            {
                ViewMode = PickerViewMode.List,
                SuggestedStartLocation = PickerLocationId.DocumentsLibrary,
                FileTypeFilter = { "*" }
            };

            IReadOnlyList<StorageFile> pickedFiles = await filePicker.PickMultipleFilesAsync();

            if (pickedFiles.Count > 0)
            {
                this.storageItems = pickedFiles;

                // Display the file names in the UI.
                string selectedFiles = String.Empty;
                for (int index = 0; index < pickedFiles.Count; index++)
                {
                    selectedFiles += pickedFiles[index].Name;

                    if (index != (pickedFiles.Count - 1))
                    {
                        selectedFiles += ", ";
                    }
                }
                this.rootPage.NotifyUser("Picked files: " + selectedFiles + ".", NotifyType.StatusMessage);

                ShareStep.Visibility = Visibility.Visible;
            }
        }
    }
}
