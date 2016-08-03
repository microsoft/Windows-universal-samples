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
using System.IO;
using System.Text;
using Windows.Storage;
using Windows.Storage.AccessCache;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Persisting access to a storage item for future use.
    /// </summary>
    public sealed partial class Scenario7 : Page
    {
        MainPage rootPage;

        const int FA_E_MAX_PERSISTED_ITEMS_REACHED = unchecked((int)0x80270220);

        public Scenario7()
        {
            this.InitializeComponent();
            AddToListButton.Click += new RoutedEventHandler(AddToListButton_Click);
            ShowListButton.Click += new RoutedEventHandler(ShowListButton_Click);
            OpenFromListButton.Click += new RoutedEventHandler(OpenFromListButton_Click);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            rootPage.ValidateFile();
        }

        private void AddToListButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFile file = rootPage.sampleFile;
            if (file != null)
            {
                if (MRURadioButton.IsChecked.Value)
                {
                    // Add the file to app MRU and possibly system MRU
                    RecentStorageItemVisibility visibility = SystemMRUCheckBox.IsChecked.Value ? RecentStorageItemVisibility.AppAndSystem : RecentStorageItemVisibility.AppOnly;
                    rootPage.mruToken = StorageApplicationPermissions.MostRecentlyUsedList.Add(file, file.Name, visibility);
                    rootPage.NotifyUser(String.Format("The file '{0}' was added to the MRU list and a token was stored.", file.Name), NotifyType.StatusMessage);
                }
                else
                {
                    try
                    {
                        rootPage.falToken = StorageApplicationPermissions.FutureAccessList.Add(file, file.Name);
                        rootPage.NotifyUser(String.Format("The file '{0}' was added to the FAL list and a token was stored.", file.Name), NotifyType.StatusMessage);
                    }
                    catch (Exception ex) when (ex.HResult == FA_E_MAX_PERSISTED_ITEMS_REACHED)
                    {
                        // A real program would call Remove() to create room in the FAL.
                        rootPage.NotifyUser(String.Format("The file '{0}' was not added to the FAL list because the FAL list is full.", file.Name), NotifyType.ErrorMessage);
                    }
                }
            }
            else
            {
                rootPage.NotifyUserFileNotExist();
            }
        }

        private void ShowListButton_Click(object sender, RoutedEventArgs e)
        {
            AccessListEntryView entries;
            string listName;
            if (MRURadioButton.IsChecked.Value)
            {
                listName = "MRU";
                entries = StorageApplicationPermissions.MostRecentlyUsedList.Entries;
            }
            else
            {
                listName = "FAL";
                entries = StorageApplicationPermissions.FutureAccessList.Entries;
            }

            if (entries.Count > 0)
            {
                StringBuilder outputText = new StringBuilder("The " + listName + " list contains the following item(s):");
                foreach (AccessListEntry entry in entries)
                {
                    outputText.AppendLine();
                    outputText.Append(entry.Metadata); // Application previously chose to store file.Name in this field
                }

                rootPage.NotifyUser(outputText.ToString(), NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("The " + listName + " list is empty.", NotifyType.ErrorMessage);
            }
        }

        private async void OpenFromListButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFile file = null;

            if (MRURadioButton.IsChecked.Value)
            {
                if (rootPage.mruToken != null)
                {
                    // When the MRU becomes full, older entries are automatically deleted, so check if the
                    // token is still valid before using it.
                    if (StorageApplicationPermissions.MostRecentlyUsedList.ContainsItem(rootPage.mruToken))
                    {
                        // Open the file via the token that was stored when adding this file into the MRU list
                        file = await StorageApplicationPermissions.MostRecentlyUsedList.GetFileAsync(rootPage.mruToken);
                    }
                    else
                    {
                        rootPage.NotifyUser("The token is no longer valid.", NotifyType.ErrorMessage);
                    }
                }
                else
                {
                    rootPage.NotifyUser("This operation requires a token. Add file to the MRU list first.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                if (rootPage.falToken != null)
                {
                    // Open the file via the token that was stored when adding this file into the FAL list.
                    // The token remains valid until we explicitly remove it.
                    file = await StorageApplicationPermissions.FutureAccessList.GetFileAsync(rootPage.falToken);
                }
                else
                {
                    rootPage.NotifyUser("This operation requires a token. Add file to the FAL list first.", NotifyType.ErrorMessage);
                }
            }

            if (file != null)
            {
                try
                {
                    // Read the file
                    string fileContent = await FileIO.ReadTextAsync(file);
                    rootPage.NotifyUser(String.Format("The file '{0}' was opened by a stored token. It contains the following text:{1}{2}", file.Name, Environment.NewLine, fileContent), NotifyType.StatusMessage);

                }
                catch (Exception ex)
                {
                    // I/O errors are reported as exceptions.
                    rootPage.NotifyUser(String.Format("Error reading file opened from list: {0}", ex.Message), NotifyType.ErrorMessage);
                }
            }
        }
    }
}
