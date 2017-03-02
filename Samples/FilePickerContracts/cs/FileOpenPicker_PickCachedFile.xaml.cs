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
using Windows.ApplicationModel;
using Windows.Foundation;
using Windows.Storage;
using Windows.Storage.Pickers.Provider;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Storage.Provider;

namespace FilePickerContracts
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class FileOpenPicker_PickCachedFile : Page
    {
        private const string id = "MyCachedFile";
        FileOpenPickerUI fileOpenPickerUI = FileOpenPickerPage.Current.fileOpenPickerUI;
        CoreDispatcher dispatcher = Window.Current.Dispatcher;

        public FileOpenPicker_PickCachedFile()
        {
            this.InitializeComponent();
            AddFileButton.Click += new RoutedEventHandler(AddFileButton_Click);
            RemoveFileButton.Click += new RoutedEventHandler(RemoveFileButton_Click);
        }

        private void UpdateButtonState(bool fileInBasket)
        {
            AddFileButton.IsEnabled = !fileInBasket;
            RemoveFileButton.IsEnabled = fileInBasket;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            UpdateButtonState(fileOpenPickerUI.ContainsFile(id));
        }

        private async void AddFileButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFile file = await Windows.Storage.ApplicationData.Current.LocalFolder.CreateFileAsync(@"CachedFile.txt", CreationCollisionOption.ReplaceExisting);
            await Windows.Storage.FileIO.WriteTextAsync(file, @"Cached file created...");
            CachedFileUpdater.SetUpdateInformation(file, "CachedFile", ReadActivationMode.BeforeAccess, WriteActivationMode.NotNeeded, CachedFileOptions.RequireUpdateOnAccess);

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

        private void RemoveFileButton_Click(object sender, RoutedEventArgs e)
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
