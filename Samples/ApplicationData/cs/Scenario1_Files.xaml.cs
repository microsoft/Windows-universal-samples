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
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Files : Page
    {
        StorageFolder localFolder = ApplicationData.Current.LocalFolder;
        int localCounter = 0;
        StorageFolder localCacheFolder = ApplicationData.Current.LocalCacheFolder;
        int localCacheCounter = 0;
        StorageFolder temporaryFolder = ApplicationData.Current.TemporaryFolder;
        int temporaryCounter = 0;

        const string filename = "sampleFile.txt";

        public Scenario1_Files()
        {
            InitializeComponent();
        }

        // Guidance for Local, LocalCache, and Temporary files.
        //
        // Files are ideal for storing large data-sets, databases, or data that is
        // in a common file-format.
        //
        // Files can exist in either the Local, LocalCache, or Temporary folders.
        // (They can also be put in Roaming folders, but the data no longer roams.)
        //
        // Local files are not synchronized, but they are backed up, and can then be restored to a 
        // machine different than where they were originally written. These should be for 
        // important files that allow the feel that the user did not lose anything
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
            StorageFile file = (await localFolder.TryGetItemAsync(filename)) as StorageFile;
            if (file != null)
            {
                string text = await FileIO.ReadTextAsync(file);

                LocalOutputTextBlock.Text = "Local Counter: " + text;

                localCounter = int.Parse(text);
            }
            else
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
            StorageFile file = (await localCacheFolder.TryGetItemAsync(filename)) as StorageFile;
            if (file != null)
            {
                string text = await FileIO.ReadTextAsync(file);

                LocalCacheOutputTextBlock.Text = "LocalCache Counter: " + text;

                localCacheCounter = int.Parse(text);
            }
            else
            {
                LocalCacheOutputTextBlock.Text = "LocalCache Counter: <not found>";
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
            StorageFile file = (await temporaryFolder.TryGetItemAsync(filename)) as StorageFile;
            if (file != null)
            {
                string text = await FileIO.ReadTextAsync(file);

                TemporaryOutputTextBlock.Text = "Temporary Counter: " + text;

                temporaryCounter = int.Parse(text);
            }
            else
            {
                TemporaryOutputTextBlock.Text = "Temporary Counter: <not found>";
            }
        }

        void DisplayOutput()
        {
            Read_Local_Counter();
            Read_LocalCache_Counter();
            Read_Temporary_Counter();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            DisplayOutput();
        }
    }
}
