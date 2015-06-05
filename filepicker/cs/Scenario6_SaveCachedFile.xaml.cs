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
using System.Collections.Generic;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Provider;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace FilePicker
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario6 : Page
    {
        MainPage rootPage = MainPage.Current;
        string fileToken = string.Empty;

        public Scenario6()
        {
            this.InitializeComponent();
            SaveFileButton.Click += new RoutedEventHandler(SaveFileButton_Click);
            WriteFileButton.Click += new RoutedEventHandler(WriteFileButton_Click);
        }

        private async void SaveFileButton_Click(object sender, RoutedEventArgs e)
        {
            // Clear previous returned file name, if it exists, between iterations of this scenario
            OutputFileName.Text = "";
            OutputFileContent.Text = "";

            FileSavePicker savePicker = new FileSavePicker();
            // Dropdown of file types the user can save the file as
            savePicker.FileTypeChoices.Add("Plain Text", new List<string>() { ".txt" });
            // Default file name if the user does not type one in or select a file to replace
            savePicker.SuggestedFileName = "New Document";
            StorageFile file = await savePicker.PickSaveFileAsync();
            if (file != null)
            {
                fileToken = Windows.Storage.AccessCache.StorageApplicationPermissions.FutureAccessList.Add(file);
                MainPage.Current.NotifyUser(String.Format(@"Received file: {0}", file.Name), NotifyType.StatusMessage);
                WriteFileButton.IsEnabled = true;
            }
            else
            {
                MainPage.Current.NotifyUser("Operation cancelled.", NotifyType.StatusMessage);;
            }
        }

        private async void WriteFileButton_Click(object sender, RoutedEventArgs e)
        {
            if (!String.IsNullOrEmpty(fileToken))
            {
                StorageFile file = await Windows.Storage.AccessCache.StorageApplicationPermissions.FutureAccessList.GetFileAsync(fileToken);
                // Prevent updates to the remote version of the file until we finish making changes and call CompleteUpdatesAsync.
                CachedFileManager.DeferUpdates(file);
                // write to file
                await FileIO.AppendTextAsync(file, String.Format(@"{0}Text Added @ {1}.", System.Environment.NewLine, DateTime.Now.ToString()));
                // Let Windows know that we're finished changing the file so the server app can update the remote version of the file.
                // Completing updates may require Windows to ask for user input.
                FileUpdateStatus status = await CachedFileManager.CompleteUpdatesAsync(file);
                switch (status)
                {
                    case FileUpdateStatus.Complete:
                        MainPage.Current.NotifyUser("File " + file.Name + " was saved.", NotifyType.StatusMessage);
                        OutputFileAsync(file);
                        break;

                    case FileUpdateStatus.CompleteAndRenamed:
                        MainPage.Current.NotifyUser("File " + file.Name + " was renamed and saved.", NotifyType.StatusMessage);
                        Windows.Storage.AccessCache.StorageApplicationPermissions.FutureAccessList.AddOrReplace(fileToken, file);
                        OutputFileAsync(file);
                        break;

                    default:
                        MainPage.Current.NotifyUser("File " + file.Name + " couldn't be saved.", NotifyType.StatusMessage);
                        break;
                }
            }
        }

        private async void OutputFileAsync(StorageFile file)
        {
            string fileContent = await FileIO.ReadTextAsync(file);
            OutputFileName.Text = String.Format(@"Received file: {0}", file.Name);
            OutputFileContent.Text = String.Format(@"File content:{0}{1}", System.Environment.NewLine, fileContent);
        }
    }
}
