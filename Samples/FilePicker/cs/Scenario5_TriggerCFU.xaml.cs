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
using Windows.Storage;
using Windows.Storage.AccessCache;
using Windows.Storage.Pickers;
using Windows.Storage.Provider;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario5 : Page
    {
        MainPage rootPage = MainPage.Current;

        StorageFile m_afterWriteFile = null;
        StorageFile m_beforeReadFile = null;
        string m_faToken;

        public Scenario5()
        {
            this.InitializeComponent();
            UpdateButtons();
        }

        private void UpdateButtons()
        {
            WriteToFileButton.IsEnabled = (m_afterWriteFile != null);
            WriteToFileWithCFUButton.IsEnabled = (m_afterWriteFile != null);

            SaveToFutureAccessListButton.IsEnabled = (m_beforeReadFile != null);
            GetFileFromFutureAccessListButton.IsEnabled = !string.IsNullOrEmpty(m_faToken);
        }

        private async void CreateFileButton_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            FileSavePicker savePicker = new FileSavePicker();
            savePicker.SuggestedStartLocation = PickerLocationId.DocumentsLibrary;

            savePicker.FileTypeChoices.Add("Plain Text", new string[] { ".txt" });
            savePicker.SuggestedFileName = "New Document";

            m_afterWriteFile = await savePicker.PickSaveFileAsync();
            if (m_afterWriteFile != null)
            {
                rootPage.NotifyUser("File created.", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("Operation cancelled.", NotifyType.ErrorMessage);
            }
            UpdateButtons();
        }

        private async void WriteToFileButton_Click(Object sender, RoutedEventArgs e)
        {
            await FileIO.WriteTextAsync(m_afterWriteFile, "The File Picker App just wrote to the file!");
            rootPage.NotifyUser("File write complete. If applicable, a WriteActivationMode.AfterWrite activation will occur in approximately 60 seconds on desktop.", NotifyType.StatusMessage);
        }

        private async void WriteToFileWithExplicitCFUButton_Click(Object sender, RoutedEventArgs e)
        {
            StorageFile file = m_afterWriteFile;
            CachedFileManager.DeferUpdates(file);
            await FileIO.WriteTextAsync(file, "The File Picker App just wrote to the file!");
            rootPage.NotifyUser("File write complete. Explicitly completing updates.", NotifyType.StatusMessage);
            FileUpdateStatus status = await CachedFileManager.CompleteUpdatesAsync(file);
            switch (status)
            {
                case FileUpdateStatus.Complete:
                    rootPage.NotifyUser($"File {file.Name} was saved.", NotifyType.StatusMessage);
                    break;

                case FileUpdateStatus.CompleteAndRenamed:
                    rootPage.NotifyUser($"File ${file.Name} was renamed and saved.", NotifyType.StatusMessage);
                    break;

                default:
                    rootPage.NotifyUser($"File ${file.Name} couldn't be saved.", NotifyType.ErrorMessage);
                    break;
            }
        }

        private async void PickAFileButton_Click(Object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            FileOpenPicker openPicker = new FileOpenPicker();
            openPicker.ViewMode = PickerViewMode.Thumbnail;
            openPicker.FileTypeFilter.Add(".txt");

            StorageFile file = await openPicker.PickSingleFileAsync();
            if (file != null)
            {
                m_beforeReadFile = file;
                rootPage.NotifyUser($"Picked file: {file.Name}", NotifyType.StatusMessage);
            }
            else
            {
                m_beforeReadFile = null;
                rootPage.NotifyUser("Operation cancelled.", NotifyType.ErrorMessage);
            }
            UpdateButtons();
        }

        private void SaveToFutureAccessListButton_Click(Object sender, RoutedEventArgs e)
        {
            m_faToken = StorageApplicationPermissions.FutureAccessList.Add(m_beforeReadFile);
            rootPage.NotifyUser("Saved to Future Access List", NotifyType.StatusMessage);
            m_beforeReadFile = null;
            UpdateButtons();
        }

        private async void GetFileFromFutureAccessListButton_Click(Object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Getting the file from the Future Access List. If applicable, a ReadActivationMode.BeforeAccess activation will occur in approximately 60 seconds on desktop.", NotifyType.StatusMessage);
            StorageFile file = await StorageApplicationPermissions.FutureAccessList.GetFileAsync(m_faToken);
            if (file != null)
            {
                rootPage.NotifyUser($"Retrieved file from Future Access List: {file.Name}", NotifyType.StatusMessage);
                m_beforeReadFile = file;
            }
            else
            {
                rootPage.NotifyUser("Unable to retrieve file from Future Access List.", NotifyType.ErrorMessage);
            }
            UpdateButtons();
        }
    }
}

