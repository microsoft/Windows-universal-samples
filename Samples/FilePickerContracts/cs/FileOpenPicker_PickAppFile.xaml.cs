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

namespace FilePickerContracts
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class FileOpenPicker_PickAppFile : Page
    {
        private const string id = "MyLocalFile";
        FileOpenPickerUI fileOpenPickerUI = FileOpenPickerPage.Current.fileOpenPickerUI;
        CoreDispatcher dispatcher = Window.Current.Dispatcher;

        public FileOpenPicker_PickAppFile()
        {
            this.InitializeComponent();
            AddLocalFileButton.Click += new RoutedEventHandler(AddLocalFileButton_Click);
            RemoveLocalFileButton.Click += new RoutedEventHandler(RemoveLocalFileButton_Click);
        }

        private void UpdateButtonState(bool fileInBasket)
        {
            AddLocalFileButton.IsEnabled = !fileInBasket;
            RemoveLocalFileButton.IsEnabled = fileInBasket;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            UpdateButtonState(fileOpenPickerUI.ContainsFile(id));
        }

        private async void AddLocalFileButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFile file = await Package.Current.InstalledLocation.GetFileAsync(@"Assets\squareTile-sdk.png");
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

        private void RemoveLocalFileButton_Click(object sender, RoutedEventArgs e)
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
