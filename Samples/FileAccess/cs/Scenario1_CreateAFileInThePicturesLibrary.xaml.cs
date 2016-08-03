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
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Creating a file.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage;

        public Scenario1()
        {
            this.InitializeComponent();
            CreateFileButton.Click += new RoutedEventHandler(CreateFileButton_Click);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private async void CreateFileButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFolder storageFolder = await KnownFolders.GetFolderForUserAsync(null /* current user */, KnownFolderId.PicturesLibrary);
            try
            {
                rootPage.sampleFile = await storageFolder.CreateFileAsync(MainPage.filename, CreationCollisionOption.ReplaceExisting);
                rootPage.NotifyUser(String.Format("The file '{0}' was created.", rootPage.sampleFile.Name), NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                // I/O errors are reported as exceptions.
                rootPage.NotifyUser(String.Format("Error creating file '{0}': {1}", MainPage.filename, ex.Message), NotifyType.ErrorMessage);
            }
        }
    }
}
