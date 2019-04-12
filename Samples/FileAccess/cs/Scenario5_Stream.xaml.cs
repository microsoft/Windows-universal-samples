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
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Writing and reading using a stream.
    /// </summary>
    public sealed partial class Scenario5 : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario5()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage.ValidateFile();
        }

        private async void WriteToStreamButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFile file = rootPage.sampleFile;
            if (file != null)
            {
                try
                {
                    string userContent = InputTextBox.Text;
                    using (StorageStreamTransaction transaction = await file.OpenTransactedWriteAsync())
                    {
                        IBuffer buffer = MainPage.GetBufferFromString(userContent);
                        await transaction.Stream.WriteAsync(buffer);
                        transaction.Stream.Size = buffer.Length; // truncate file
                        await transaction.CommitAsync();
                        rootPage.NotifyUser($"The following text was written to '{file.Name}' using a stream:\n{userContent}", NotifyType.StatusMessage);
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

        private async void ReadFromStreamButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFile file = rootPage.sampleFile;
            if (file != null)
            {
                try
                {
                    using (IRandomAccessStream readStream = await file.OpenAsync(FileAccessMode.Read))
                    {
                        UInt64 size64 = readStream.Size;
                        if (size64 <= UInt32.MaxValue)
                        {
                            UInt32 size32 = (UInt32)size64;
                            IBuffer buffer = new Windows.Storage.Streams.Buffer(size32);
                            buffer = await readStream.ReadAsync(buffer, size32, InputStreamOptions.None);
                            string fileContent = MainPage.GetStringFromBuffer(buffer);
                            rootPage.NotifyUser($"The following text was read from '{file.Name}' using a stream:\n{fileContent}", NotifyType.StatusMessage);
                        }
                        else
                        {
                            rootPage.NotifyUser($"File {file.Name} is too big for ReadAsync to read in a single chunk. Files larger than 4GB need to be broken into multiple chunks to be read by ReadAsync.", NotifyType.ErrorMessage);
                        }
                    }
                }
                catch (FileNotFoundException)
                {
                    rootPage.NotifyUserFileNotExist();
                }
                catch (Exception ex) when (ex.HResult == MainPage.E_NO_UNICODE_TRANSLATION)
                {
                    rootPage.NotifyUser("File is not UTF-8 encoded.", NotifyType.ErrorMessage);
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
