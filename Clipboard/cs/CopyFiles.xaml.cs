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
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class CopyFiles : Page
    {
        MainPage rootPage = MainPage.Current;

        public CopyFiles()
        {
            this.InitializeComponent();
            this.Init();
        }

        
        void Init()
        {
            CopyButton.Click += new RoutedEventHandler(CopyButton_Click);
            PasteButton.Click += new RoutedEventHandler(PasteButton_Click);
        }

        
        
        async void CopyButton_Click(object sender, RoutedEventArgs e)
        {
            OutputText.Text = "Storage Items: ";
            var filePicker = new FileOpenPicker
            {
                ViewMode = PickerViewMode.List,
                FileTypeFilter = { "*" }
            };

            var storageItems = await filePicker.PickMultipleFilesAsync();
            if (storageItems.Count > 0)
            {
                OutputText.Text += storageItems.Count + " file(s) are copied into clipboard";
                var dataPackage = new DataPackage();
                dataPackage.SetStorageItems(storageItems);

                // Request a copy operation from targets that support different file operations, like File Explorer
                dataPackage.RequestedOperation = DataPackageOperation.Copy;
                try
                {
                    Windows.ApplicationModel.DataTransfer.Clipboard.SetContent(dataPackage);
                }
                catch (Exception ex)
                {
                    // Copying data to Clipboard can potentially fail - for example, if another application is holding Clipboard open
                    rootPage.NotifyUser("Error copying content to Clipboard: " + ex.Message + ". Try again", NotifyType.ErrorMessage);
                }
            }
            else
            {
                OutputText.Text += "No file was selected.";
            }
        }

        async void PasteButton_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            // Get data package from clipboard
            var dataPackageView = Windows.ApplicationModel.DataTransfer.Clipboard.GetContent();

            if (dataPackageView.Contains(StandardDataFormats.StorageItems))
            {
                IReadOnlyList<IStorageItem> storageItems = null;
                try
                {
                    storageItems = await dataPackageView.GetStorageItemsAsync();
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser("Error retrieving file(s) from Clipboard: " + ex.Message, NotifyType.ErrorMessage);
                }

                if (storageItems != null)
                {
                    OutputText.Text = "Pasting following" + storageItems.Count + " file(s) to the folder " + ApplicationData.Current.LocalFolder.Path + Environment.NewLine;
                    var operation = dataPackageView.RequestedOperation;
                    OutputText.Text += "Requested Operation: ";
                    switch (operation)
                    {
                        case DataPackageOperation.Copy:
                            OutputText.Text += "Copy";
                            break;
                        case DataPackageOperation.Link:
                            OutputText.Text += "Link";
                            break;
                        case DataPackageOperation.Move:
                            OutputText.Text += "Move";
                            break;
                        case DataPackageOperation.None:
                            OutputText.Text += "None";
                            break;
                        default:
                            OutputText.Text += "Unknown";
                            break;
                    }
                    OutputText.Text += Environment.NewLine;

                    // Iterate through each item in the collection
                    foreach (var storageItem in storageItems)
                    {
                        var file = storageItem as StorageFile;
                        if (file != null)
                        {
                            // Copy the file
                            var newFile = await file.CopyAsync(ApplicationData.Current.LocalFolder, file.Name, NameCollisionOption.ReplaceExisting);
                            if (newFile != null)
                            {
                                OutputText.Text += file.Name + Environment.NewLine;
                            }
                        }
                        else
                        {
                            var folder = storageItem as StorageFolder;
                            if (folder != null)
                            {
                                // Skipping folders for brevity sake
                                OutputText.Text += folder.Path + " is a folder, skipping" + Environment.NewLine;
                            }
                        }
                    }
                }
            }
            else
            {
                OutputText.Text = "StorageItems format is not available in clipboard";
            }
        }

            }
}
