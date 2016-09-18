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
using System.IO;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Copying a file.
    /// </summary>
    public sealed partial class Scenario8 : Page
    {
        MainPage rootPage;

        public Scenario8()
        {
            this.InitializeComponent();
            CopyFileButton.Click += new RoutedEventHandler(CopyFileButton_Click);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            rootPage.ValidateFile();
        }

        private async void CopyFileButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFile file = rootPage.sampleFile;
            if (file != null)
            {
                try
                {
                    StorageFolder picturesLibrary = await KnownFolders.GetFolderForUserAsync(null /* current user */, KnownFolderId.PicturesLibrary);
                    StorageFile fileCopy = await file.CopyAsync(picturesLibrary, "sample - Copy.dat", NameCollisionOption.ReplaceExisting);
                    rootPage.NotifyUser(String.Format("The file '{0}' was copied and the new file was named '{1}'.", file.Name, fileCopy.Name), NotifyType.StatusMessage);
                }
                catch (FileNotFoundException)
                {
                    rootPage.NotifyUserFileNotExist();
                }
                catch (Exception ex)
                {
                    // I/O errors are reported as exceptions.
                    rootPage.NotifyUser(String.Format("Error copying file '{0}': {1}", file.Name, ex.Message), NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUserFileNotExist();
            }
        }
    }
}
