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
        MainPage rootPage;

        public Scenario3()
        {
            this.InitializeComponent();
            WriteTextButton.Click += new RoutedEventHandler(WriteTextButton_Click);
            ReadTextButton.Click += new RoutedEventHandler(ReadTextButton_Click);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
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
                    if (!String.IsNullOrEmpty(userContent))
                    {
                        await FileIO.WriteTextAsync(file, userContent);
                        rootPage.NotifyUser(String.Format("The following text was written to '{0}':{1}{2}", file.Name, Environment.NewLine, userContent), NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser("The text box is empty, please write something and then click 'Write' again.", NotifyType.ErrorMessage);
                    }
                }
                catch (FileNotFoundException)
                {
                    rootPage.NotifyUserFileNotExist();
                }
                catch (Exception ex)
                {
                    // I/O errors are reported as exceptions.
                    rootPage.NotifyUser(String.Format("Error writing to '{0}': {1}", file.Name, ex.Message), NotifyType.ErrorMessage);
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
                    rootPage.NotifyUser(String.Format("The following text was read from '{0}':{1}{2}", file.Name, Environment.NewLine, fileContent), NotifyType.StatusMessage);
                }
                catch (FileNotFoundException)
                {
                    rootPage.NotifyUserFileNotExist();
                }
                catch (Exception ex)
                {
                    // I/O errors are reported as exceptions.
                    rootPage.NotifyUser(String.Format("Error reading from '{0}': {1}", file.Name, ex.Message), NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUserFileNotExist();
            }
        }
    }
}
