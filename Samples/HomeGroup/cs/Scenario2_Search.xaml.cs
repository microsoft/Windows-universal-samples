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
    public sealed partial class Scenario2_Search : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario2_Search()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// This is the click handler for the Search button
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void Search_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);
            OutputProgressRing.IsActive = true;
            Windows.Storage.Search.QueryOptions queryOptions = new Windows.Storage.Search.QueryOptions(Windows.Storage.Search.CommonFileQuery.OrderBySearchRank, null);
            queryOptions.UserSearchFilter = QueryInputBox.Text;

            // This try/catch block is for scenarios where the HomeGroup Known Folder is not available.
            try
            {
                Windows.Storage.Search.StorageFileQueryResult fileQuery = Windows.Storage.KnownFolders.HomeGroup.CreateFileQueryWithOptions(queryOptions);
                fileQuery = Windows.Storage.KnownFolders.PicturesLibrary.CreateFileQueryWithOptions(queryOptions);
                System.Collections.Generic.IReadOnlyList<Windows.Storage.StorageFile> files = await fileQuery.GetFilesAsync();

                string outputString = "Files found: " + files.Count.ToString() + "\n";
                foreach (Windows.Storage.StorageFile file in files)
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
