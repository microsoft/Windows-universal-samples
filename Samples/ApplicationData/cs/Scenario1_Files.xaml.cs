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
using SDKTemplate;
using System;
using Windows.Storage;

namespace ApplicationDataSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Files : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        StorageFolder localFolder = null;
        int localCounter = 0;
        StorageFolder localCacheFolder = null;
        int localCacheCounter = 0;
        StorageFolder roamingFolder = null;
        int roamingCounter = 0;
        StorageFolder temporaryFolder = null;
        int temporaryCounter = 0;

        const string filename = "sampleFile.txt";

        public Files()
        {
            this.InitializeComponent();

            localFolder = ApplicationData.Current.LocalFolder;
            localCacheFolder = ApplicationData.Current.LocalCacheFolder;
            roamingFolder = ApplicationData.Current.RoamingFolder;
            temporaryFolder = ApplicationData.Current.TemporaryFolder;

            DisplayOutput();
        }

        // Guidance for Local, LocalCache, Roaming, and Temporary files.
        //
        // Files are ideal for storing large data-sets, databases, or data that is
        // in a common file-format.
        //
        // Files can exist in either the Local, LocalCache, Roaming, or Temporary folders.
        //
        // Roaming files will be synchronized across machines on which the user has
        // singed in with a connected account.  Roaming of files is not instant; the
        // system weighs several factors when determining when to send the data.  Usage
        // of roaming data should be kept below the quota (available via the 
        // RoamingStorageQuota property), or else roaming of data will be suspended.
        // Files cannot be roamed while an application is writing to them, so be sure
        // to close your application's file objects when they are no longer needed.
        //
        // Local files are not synchronized, but are backed up, and can then be restored to a 
        // machine different than where they were originally written. These should be for 
        // important files that allow the feel that the user did not loose anything
        // when they restored to a new device.
        //
        // Temporary files are subject to deletion when not in use.  The system 
        // considers factors such as available disk capacity and the age of a file when
        // determining when or whether to delete a temporary file.
        //
        // LocalCache files are for larger files that can be recreated by the app, and for
        // machine specific or private files that should not be restored to a new device.

        async void Increment_Local_Click(Object sender, RoutedEventArgs e)
        {
            localCounter++;

            StorageFile file = await localFolder.CreateFileAsync(filename, CreationCollisionOption.ReplaceExisting);
            await FileIO.WriteTextAsync(file, localCounter.ToString());

            Read_Local_Counter();
        }

        async void Read_Local_Counter()
        {
            try
            {
                StorageFile file = await localFolder.GetFileAsync(filename);
                string text = await FileIO.ReadTextAsync(file);

                LocalOutputTextBlock.Text = "Local Counter: " + text;

                localCounter = int.Parse(text);
            }
            catch (Exception)
            {
                LocalOutputTextBlock.Text = "Local Counter: <not found>";
            }
        }

        async void Increment_LocalCache_Click(Object sender, RoutedEventArgs e)
        {
            localCacheCounter++;

            StorageFile file = await localCacheFolder.CreateFileAsync(filename, CreationCollisionOption.ReplaceExisting);
            await FileIO.WriteTextAsync(file, localCacheCounter.ToString());

            Read_LocalCache_Counter();
        }

        async void Read_LocalCache_Counter()
        {          
            try
            {
                StorageFile file = await localCacheFolder.GetFileAsync(filename);
                string text = await FileIO.ReadTextAsync(file);

                LocalCacheOutputTextBlock.Text = "LocalCache Counter: " + text;

                localCacheCounter = int.Parse(text);
            }
            catch (Exception)
            {
                LocalCacheOutputTextBlock.Text = "LocalCache Counter: <not found>";
            }
        }

        async void Increment_Roaming_Click(Object sender, RoutedEventArgs e)
        {
            roamingCounter++;

            StorageFile file = await roamingFolder.CreateFileAsync(filename, CreationCollisionOption.ReplaceExisting);
            await FileIO.WriteTextAsync(file, roamingCounter.ToString());

            Read_Roaming_Counter();
        }

        async void Read_Roaming_Counter()
        {
            try
            {
                StorageFile file = await roamingFolder.GetFileAsync(filename);
                string text = await FileIO.ReadTextAsync(file);

                RoamingOutputTextBlock.Text = "Roaming Counter: " + text;

                roamingCounter = int.Parse(text);
            }
            catch (Exception)
            {
                RoamingOutputTextBlock.Text = "Roaming Counter: <not found>";
            }
        }

        async void Increment_Temporary_Click(Object sender, RoutedEventArgs e)
        {
            temporaryCounter++;

            StorageFile file = await temporaryFolder.CreateFileAsync(filename, CreationCollisionOption.ReplaceExisting);
            await FileIO.WriteTextAsync(file, temporaryCounter.ToString());

            Read_Temporary_Counter();
        }

        async void Read_Temporary_Counter()
        {
            try
            {
                StorageFile file = await temporaryFolder.GetFileAsync(filename);
                string text = await FileIO.ReadTextAsync(file);

                TemporaryOutputTextBlock.Text = "Temporary Counter: " + text;

                temporaryCounter = int.Parse(text);
            }
            catch (Exception)
            {
                TemporaryOutputTextBlock.Text = "Temporary Counter: <not found>";
            }
        }

        void DisplayOutput()
        {
            Read_Local_Counter();
            Read_LocalCache_Counter();
            Read_Roaming_Counter();
            Read_Temporary_Counter();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }
    }
}