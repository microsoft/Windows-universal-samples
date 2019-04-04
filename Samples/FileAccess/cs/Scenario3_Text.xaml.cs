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
    /// Writing and reading text in a file.
    /// </summary>
    public sealed partial class Scenario3 : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario3()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage.ValidateFile();
        }

        private async void WriteTextButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFile file = rootPage.sampleFile;
            if (file != null)
            {
                try
                {
                    string userContent = InputTextBox.Text;
                    await FileIO.WriteTextAsync(file, userContent);
                    rootPage.NotifyUser($"The following text was written to '{file.Name}':\n{userContent}", NotifyType.StatusMessage);
                }
                catch (FileNotFoundException)
                {
                    rootPage.NotifyUserFileNotExist();
                }
                catch (Exception ex)
                {
                    // I/O errors are reported as exceptions.
                    rootPage.NotifyUser($"Error writing to '{file.Name}': {ex.Message}", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUserFileNotExist();
            }
        }

        private async void ReadTextButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFile file = rootPage.sampleFile;
            if (file != null)
            {
                try
                {
                    string fileContent = await FileIO.ReadTextAsync(file);
                    rootPage.NotifyUser($"The following text was read from '{file.Name}':\n{fileContent}", NotifyType.StatusMessage);
                }
                catch (FileNotFoundException)
                {
                    rootPage.NotifyUserFileNotExist();
                }
                catch (Exception ex) when (ex.HResult == MainPage.E_NO_UNICODE_TRANSLATION)
                {
                    rootPage.NotifyUser("Could not decode file to Unicode.", NotifyType.ErrorMessage);
                }
                catch (Exception ex)
                {
                    // I/O errors are reported as exceptions.
                    rootPage.NotifyUser($"Error reading from '{file.Name}': {ex.Message}", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUserFileNotExist();
            }
        }
    }
}
