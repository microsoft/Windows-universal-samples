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
using Windows.Storage;
using Windows.Storage.Search;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario4_SearchUser : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario4_SearchUser()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        async protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            try
            {
                IReadOnlyList<StorageFolder> folders = await KnownFolders.HomeGroup.GetFoldersAsync();
                if ((folders != null) && (folders.Count != 0))
                {
                    foreach (StorageFolder folder in folders)
                    {
                        // Create a button for each user in the HomeGroup.
                        // The DataContext is the folder that represents that user's files.
                        Button button = new Button();
                        button.DataContext = folder;
                        button.Style = (Style)Resources["UserButtonStyle"];
                        button.Content = folder.Name;
                        button.Click += UserButton_Click;
                        UserGrid.Items.Add(button);
                    }
                }
                else
                {
                    rootPage.NotifyUser("No HomeGroup users found. Make sure you are joined to a HomeGroup and there is someone sharing.", NotifyType.ErrorMessage);
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for the button that represents a user.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void UserButton_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            if (b != null)
            {
                OutputProgressRing.IsActive = true;

                // Get the folder that represents the user's files, which wwe saved as the DataContext.
                StorageFolder folder = (StorageFolder)b.DataContext;

                // This try/catch block is for scenarios where the HomeGroup Known Folder is not available.
                try
                {
                    // Search for all files in that folder.
                    QueryOptions queryOptions = new QueryOptions(CommonFileQuery.OrderBySearchRank, null);
                    queryOptions.UserSearchFilter = "*";
                    StorageFileQueryResult queryResults = folder.CreateFileQueryWithOptions(queryOptions);
                    IReadOnlyList<StorageFile> files = await queryResults.GetFilesAsync();

                    string outputString = string.Format("Files shared by {0}: {1}\n", folder.Name, files.Count);
                    foreach (StorageFile file in files)
                    {
                        outputString += file.Name + "\n";
                    }
                    rootPage.NotifyUser(outputString, NotifyType.StatusMessage);
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
                }
                OutputProgressRing.IsActive = false;
            }
        }
    }
}
