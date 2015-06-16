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
    public sealed partial class SearchHomeGroup : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public SearchHomeGroup()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }


        /// <summary>
        /// This is the click handler for the Search button
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void Search_Click(object sender, RoutedEventArgs e)
        {
            OutputProgressRing.IsActive = true;
            Windows.Storage.Search.QueryOptions queryOptions = new Windows.Storage.Search.QueryOptions(Windows.Storage.Search.CommonFileQuery.OrderBySearchRank, null);
            queryOptions.UserSearchFilter = QueryInputBox.Text;

            // This try/catch block is for scenarios where the Homegroup Known Folder is not available.
            try
            {
                Windows.Storage.Search.StorageFileQueryResult queryResults = Windows.Storage.KnownFolders.HomeGroup.CreateFileQueryWithOptions(queryOptions);
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
            catch (Exception ex)
            {
                rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
            }
            OutputProgressRing.IsActive = false;
        }
    }
}
