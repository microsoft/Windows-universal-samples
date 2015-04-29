//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using System;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class SearchByUser : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public SearchByUser()
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
                System.Collections.Generic.IReadOnlyList<Windows.Storage.StorageFolder> hgFolders = await Windows.Storage.KnownFolders.HomeGroup.GetFoldersAsync();
                System.Collections.Generic.IEnumerator<Windows.Storage.StorageFolder> hgFoldersEnumerator = hgFolders.GetEnumerator();

                // arbitrary limit to match number of buttons created
                int maxUsers = (hgFolders.Count <= 4) ? hgFolders.Count : 4;

                for (int user = 0; user < maxUsers; user++)
                {
                    // We've got a user, name a button after them and make it visible
                    hgFoldersEnumerator.MoveNext();
                    switch (user)
                    {
                        case 0:
                            User0.Content = hgFoldersEnumerator.Current.Name;
                            User0.Visibility = Visibility.Visible;
                            break;
                        case 1:
                            User1.Content = hgFoldersEnumerator.Current.Name;
                            User1.Visibility = Visibility.Visible;
                            break;
                        case 2:
                            User2.Content = hgFoldersEnumerator.Current.Name;
                            User2.Visibility = Visibility.Visible;
                            break;
                        case 3:
                            User3.Content = hgFoldersEnumerator.Current.Name;
                            User3.Visibility = Visibility.Visible;
                            break;
                        default:
                            break;
                    }
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for all four buttons in this example.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void Default_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            if (b != null)
            {
                OutputProgressRing.IsActive = true;

                // Each visible button was previously set to display a homegroup user's name
                string userName = b.Content.ToString();

                // This try/catch block is for scenarios where the Homegroup Known Folder is not available.
                try
                {
                    System.Collections.Generic.IReadOnlyList<Windows.Storage.StorageFolder> hgFolders = await Windows.Storage.KnownFolders.HomeGroup.GetFoldersAsync();
                    bool userFound = false;

                    foreach (Windows.Storage.StorageFolder folder in hgFolders)
                    {
                        if (folder.DisplayName == userName)
                        {
                            // We've found the folder belonging to the target user; search for all files under it
                            userFound = true;
                            Windows.Storage.Search.QueryOptions queryOptions = new Windows.Storage.Search.QueryOptions(Windows.Storage.Search.CommonFileQuery.OrderBySearchRank, null);
                            queryOptions.UserSearchFilter = "*";
                            Windows.Storage.Search.StorageFileQueryResult queryResults = folder.CreateFileQueryWithOptions(queryOptions);
                            System.Collections.Generic.IReadOnlyList<Windows.Storage.StorageFile> files = await queryResults.GetFilesAsync();

                            if (files.Count > 0)
                            {
                                string outputString = (files.Count == 1) ? "One file found\n\n" : files.Count.ToString() + " files found\n\n";
                                foreach (Windows.Storage.StorageFile file in files)
                                {
                                    outputString += file.Name + "\n";
                                }
                                rootPage.NotifyUser(outputString, NotifyType.StatusMessage);
                            }
                            else
                            {
                                rootPage.NotifyUser("No files found.", NotifyType.StatusMessage);
                            }
                        }
                    }

                    if (!userFound)
                    {
                        rootPage.NotifyUser("The user " + userName + " was not found on the HomeGroup.", NotifyType.ErrorMessage);
                    }
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
