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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace FilePickerContracts
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class FileSavePicker_SaveToAppStorage : Page
    {
        FileSavePickerUI fileSavePickerUI = FileSavePickerPage.Current.fileSavePickerUI;

        public FileSavePicker_SaveToAppStorage()
        {
            this.InitializeComponent();
        }

        private async void OnTargetFileRequested(FileSavePickerUI sender, TargetFileRequestedEventArgs e)
        {
            // This scenario demonstrates how to handle the TargetFileRequested event on the background thread on which it was raised

            // Requesting a deferral allows the app to call another asynchronous method and complete the request at a later time
            var deferral = e.Request.GetDeferral();

            e.Request.TargetFile = await ApplicationData.Current.LocalFolder.CreateFileAsync(sender.FileName, CreationCollisionOption.GenerateUniqueName);

            // Complete the deferral to let the Picker know the request is finished
            deferral.Complete();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            fileSavePickerUI.TargetFileRequested += new TypedEventHandler<FileSavePickerUI, TargetFileRequestedEventArgs>(OnTargetFileRequested);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            fileSavePickerUI.TargetFileRequested -= new TypedEventHandler<FileSavePickerUI, TargetFileRequestedEventArgs>(OnTargetFileRequested);
        }
    }
}
