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
using System.Threading.Tasks;
using Windows.ApplicationModel.Activation;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario3_ReceiveFile : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        string Extension = "alsdkcs";

        public Scenario3_ReceiveFile()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            // Display the result of the file activation if we got here as a result of being activated for a file.
            var args = e.Parameter as FileActivatedEventArgs;

            if (args != null)
            {
                string output = "Files received: " + args.Files.Count + "\n";
                foreach (StorageFile file in args.Files)
                {
                    output = output + file.Name + "\n";
                }

                if (args.NeighboringFilesQuery != null)
                {
                    // If supported, we receive the pictures from the same folder as the file that was launched
                    // because we declared the picturesLibrary capability. The order of the neighboring
                    // files matches the sort order of the folder from which the file was launched.
                    IReadOnlyList<StorageFile> neighboringFiles = await args.NeighboringFilesQuery.GetFilesAsync();
                    if (neighboringFiles.Count > 0)
                    {
                        output = output + "\nNeighboring files: " + neighboringFiles.Count + "\n";
                        int i;
                        for (i = 0; i < Math.Min(neighboringFiles.Count, 3); i++)
                        {
                            output = output + neighboringFiles[i].Name + "\n";
                        }
                        int remaining = neighboringFiles.Count - i;
                        if (remaining > 0)
                        {
                            output = output + "and " + remaining + " more.";
                        }
                    }
                }
                rootPage.NotifyUser(output, NotifyType.StatusMessage);
            }
        }

        private async void CreateTestFile()
        {
            StorageFolder folder = KnownFolders.PicturesLibrary;
            await folder.CreateFileAsync("Test " + Extension + " file." + Extension, CreationCollisionOption.ReplaceExisting);
            await Windows.System.Launcher.LaunchFolderAsync(folder);
        }

        private async void RemoveTestFile()
        {
            StorageFolder folder = KnownFolders.PicturesLibrary;
            try
            {
                StorageFile file = await folder.GetFileAsync("Test " + Extension + " file." + Extension);
                await file.DeleteAsync();
            }
            catch (Exception)
            {
                // File I/O errors are reported as exceptions.
                // Ignore errors here.
            }
        }
    }
}