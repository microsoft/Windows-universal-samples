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
using Windows.Storage.Provider;

namespace FilePickerContracts
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class FileSavePicker_SaveToCachedFile : Page
    {
        FileSavePickerUI fileSavePickerUI = FileSavePickerPage.Current.fileSavePickerUI;

        public FileSavePicker_SaveToCachedFile()
        {
            this.InitializeComponent();
        }

        private async void OnTargetFileRequested(FileSavePickerUI sender, TargetFileRequestedEventArgs e)
        {
            // This scenario demonstrates how to handle the TargetFileRequested event on the background thread on which it was raised

            // Requesting a deferral allows the app to call another asynchronous method and complete the request at a later time
            var deferral = e.Request.GetDeferral();

            StorageFile file = await ApplicationData.Current.LocalFolder.CreateFileAsync(sender.FileName, CreationCollisionOption.ReplaceExisting);
            CachedFileUpdater.SetUpdateInformation(file, "CachedFile", ReadActivationMode.NotNeeded, WriteActivationMode.AfterWrite, CachedFileOptions.RequireUpdateOnAccess);
            e.Request.TargetFile = file;

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
