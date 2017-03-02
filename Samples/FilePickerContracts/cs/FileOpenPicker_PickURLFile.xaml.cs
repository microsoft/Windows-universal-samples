//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using SDKTemplate;

using System;
using Windows.Foundation;
using Windows.Storage;
using Windows.Storage.Pickers.Provider;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace FilePickerContracts
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class FileOpenPicker_PickURLFile : Page
    {
        private const string id = "MyUrlFile";
        FileOpenPickerUI fileOpenPickerUI = FileOpenPickerPage.Current.fileOpenPickerUI;
        CoreDispatcher dispatcher = Window.Current.Dispatcher;

        public FileOpenPicker_PickURLFile()
        {
            this.InitializeComponent();
            AddURLFileButton.Click += new RoutedEventHandler(AddUriFileButton_Click);
            RemoveURLFileButton.Click += new RoutedEventHandler(RemoveUriFileButton_Click);
        }

        private void UpdateButtonState(bool fileInBasket)
        {
            AddURLFileButton.IsEnabled = !fileInBasket;
            RemoveURLFileButton.IsEnabled = fileInBasket;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            UpdateButtonState(fileOpenPickerUI.ContainsFile(id));
        }

        private async void AddUriFileButton_Click(object sender, RoutedEventArgs e)
        {
            const string filename = @"URI.png"; // This will be used as the filename of the StorageFile object that references the specified URI

            FileOpenPickerPage.Current.NotifyUser("", NotifyType.StatusMessage);

            Uri uri = null;
            try
            {
                uri = new Uri(URLInput.Text);
            }
            catch (FormatException)
            {
                FileOpenPickerPage.Current.NotifyUser("Please enter a valid URL.", NotifyType.ErrorMessage);
            }

            if (uri != null)
            {
                StorageFile file = await StorageFile.CreateStreamedFileFromUriAsync(filename, uri, RandomAccessStreamReference.CreateFromUri(uri));
                bool inBasket;
                switch (fileOpenPickerUI.AddFile(id, file))
                {
                    case AddFileResult.Added:
                    case AddFileResult.AlreadyAdded:
                        inBasket = true;
                        OutputTextBlock.Text = Status.FileAdded;
                        break;

                    default:
                        inBasket = false;
                        OutputTextBlock.Text = Status.FileAddFailed;
                        break;
                }
                UpdateButtonState(inBasket);
            }
        }

        private void RemoveUriFileButton_Click(object sender, RoutedEventArgs e)
        {
            if (fileOpenPickerUI.ContainsFile(id))
            {
                fileOpenPickerUI.RemoveFile(id);
                OutputTextBlock.Text = Status.FileRemoved;
            }
            UpdateButtonState(false);
        }
    }
}
