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
                else if (FALRadioButton.IsChecked.Value)
                {
                    rootPage.falToken = StorageApplicationPermissions.FutureAccessList.Add(file, file.Name);
                    rootPage.NotifyUser(String.Format("The file '{0}' was added to the FAL list and a token was stored.", file.Name), NotifyType.StatusMessage);
                }
            }
            else
            {
                rootPage.NotifyUserFileNotExist();
            }
        }

        private void ShowListButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFile file = rootPage.sampleFile;
            if (file != null)
            {
                if (MRURadioButton.IsChecked.Value)
                {
                    AccessListEntryView entries = StorageApplicationPermissions.MostRecentlyUsedList.Entries;
                    if (entries.Count > 0)
                    {
                        StringBuilder outputText = new StringBuilder("The MRU list contains the following item(s):");
                        foreach (AccessListEntry entry in entries)
                        {
                            outputText.AppendLine();
                            outputText.Append(entry.Metadata); // Application previously chose to store file.Name in this field
                        }

                        rootPage.NotifyUser(outputText.ToString(), NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser("The MRU list is empty, please select 'Most Recently Used' list and click 'Add to List' to add a file to the MRU list.", NotifyType.ErrorMessage);
                    }
                }
                else if (FALRadioButton.IsChecked.Value)
                {
                    AccessListEntryView entries = StorageApplicationPermissions.FutureAccessList.Entries;
                    if (entries.Count > 0)
                    {
                        StringBuilder outputText = new StringBuilder("The FAL list contains the following item(s):");
                        foreach (AccessListEntry entry in entries)
                        {
                            outputText.AppendLine();
                            outputText.Append(entry.Metadata); // Application previously chose to store file.Name in this field
                        }

                        rootPage.NotifyUser(outputText.ToString(), NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser("The FAL list is empty, please select 'Future Access List' list and click 'Add to List' to add a file to the FAL list.", NotifyType.ErrorMessage);
                    }
                }
            }
            else
            {
                rootPage.NotifyUserFileNotExist();
            }
        }

        private async void OpenFromListButton_Click(object sender, RoutedEventArgs e)
        {
            if (rootPage.sampleFile != null)
            {
                try
                {
                    if (MRURadioButton.IsChecked.Value)
                    {
                        if (rootPage.mruToken != null)
                        {
                            // Open the file via the token that was stored when adding this file into the MRU list
                            StorageFile file = await StorageApplicationPermissions.MostRecentlyUsedList.GetFileAsync(rootPage.mruToken);

                            // Read the file
                            string fileContent = await FileIO.ReadTextAsync(file);
                            rootPage.NotifyUser(String.Format("The file '{0}' was opened by a stored token from the MRU list, it contains the following text:{1}{2}", file.Name, Environment.NewLine, fileContent), NotifyType.StatusMessage);
                        }
                        else
                        {
                            rootPage.NotifyUser("The MRU list is empty, please select 'Most Recently Used' list and click 'Add to List' to add a file to the MRU list.", NotifyType.ErrorMessage);
                        }
                    }
                    else if (FALRadioButton.IsChecked.Value)
                    {
                        if (rootPage.falToken != null)
                        {
                            // Open the file via the token that was stored when adding this file into the FAL list
                            StorageFile file = await StorageApplicationPermissions.FutureAccessList.GetFileAsync(rootPage.falToken);

                            // Read the file
                            string fileContent = await FileIO.ReadTextAsync(file);
                            rootPage.NotifyUser(String.Format("The file '{0}' was opened by a stored token from the FAL list, it contains the following text:{1}{2}", file.Name, Environment.NewLine, fileContent), NotifyType.StatusMessage);
                        }
                        else
                        {
                            rootPage.NotifyUser("The FAL list is empty, please select 'Future Access List' list and click 'Add to List' to add a file to the FAL list.", NotifyType.ErrorMessage);
                        }
                    }
                }
                catch (FileNotFoundException)
                {
                    rootPage.NotifyUserFileNotExist();
                }
            }
            else
            {
                rootPage.NotifyUserFileNotExist();
            }
        }
    }
}
