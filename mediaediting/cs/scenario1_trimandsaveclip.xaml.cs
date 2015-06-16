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
using System.Collections.Generic;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Storage;
using Windows.Media.Editing;
using Windows.Media.Transcoding;
using Windows.UI.Core;
using SDKTemplate;
using Windows.Storage.AccessCache;
using Windows.Media.Core;

namespace MediaEditingSample
{
    public sealed partial class Scenario1_TrimAndSaveClip : Page
    {
        private StorageItemAccessList storageItemAccessList;
        private MainPage rootPage;
        private MediaComposition composition;
        private StorageFile pickedFile;
        private MediaStreamSource mediaStreamSource;

        public Scenario1_TrimAndSaveClip()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            // Make sure we don't run out of entries in StoreItemAccessList.
            // As we don't need to persist this across app sessions/pages, clearing
            // every time should be sufficient for us.
            storageItemAccessList = StorageApplicationPermissions.FutureAccessList;
            storageItemAccessList.Clear();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            mediaElement.Source = null;
            mediaStreamSource = null;
            base.OnNavigatedFrom(e);
        }

        private async void ChooseFile_Click(object sender, RoutedEventArgs e)
        {
            // Get file
            var picker = new Windows.Storage.Pickers.FileOpenPicker();
            picker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.VideosLibrary;
            picker.FileTypeFilter.Add(".mp4");
            pickedFile = await picker.PickSingleFileAsync();
            if (pickedFile == null)
            {
                rootPage.NotifyUser("File picking cancelled", NotifyType.ErrorMessage);
                return;
            }

            // These files could be picked from a location that we won't have access to later
            // (especially if persisting the MediaComposition to disk and loading it later). 
            // Use the StorageItemAccessList in order to keep access permissions to that
            // file for later use. Be aware that this access list needs to be cleared
            // periodically or the app will run out of entries.
            storageItemAccessList.Add(pickedFile);

            mediaElement.SetSource(await pickedFile.OpenReadAsync(), pickedFile.ContentType);
            trimClip.IsEnabled = true;
        }

        private async void TrimClip_Click(object sender, RoutedEventArgs e)
        {
            // Trim the front and back 25% from the clip
            var clip = await MediaClip.CreateFromFileAsync(pickedFile);
            clip.TrimTimeFromStart = new TimeSpan((long)(clip.OriginalDuration.Ticks * 0.25));
            clip.TrimTimeFromEnd = new TimeSpan((long)(clip.OriginalDuration.Ticks * 0.25));

            // Create a MediaComposition containing the clip and set it on the MediaElement.
            composition = new MediaComposition();
            composition.Clips.Add(clip);
            mediaElement.Position = TimeSpan.Zero;
            mediaStreamSource = composition.GeneratePreviewMediaStreamSource((int)mediaElement.ActualWidth, (int)mediaElement.ActualHeight);
            mediaElement.SetMediaStreamSource(mediaStreamSource);
            rootPage.NotifyUser("Clip trimmed", NotifyType.StatusMessage);
            save.IsEnabled = true;
        }

        private async void Save_Click(object sender, RoutedEventArgs e)
        {           
            EnableButtons(false);
            rootPage.NotifyUser("Requesting file to save to", NotifyType.StatusMessage);

            var picker = new Windows.Storage.Pickers.FileSavePicker();
            picker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.VideosLibrary;
            picker.FileTypeChoices.Add("MP4 files", new List<string>() { ".mp4" });
            picker.SuggestedFileName = "TrimmedClip.mp4";

            StorageFile file = await picker.PickSaveFileAsync();
            if (file != null)
            {
                var saveOperation = composition.RenderToFileAsync(file, MediaTrimmingPreference.Precise);
                saveOperation.Progress = new AsyncOperationProgressHandler<TranscodeFailureReason, double>(async (info, progress) =>
                {
                    await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(() =>
                    {
                        rootPage.NotifyUser(string.Format("Saving file... Progress: {0:F0}%", progress), NotifyType.StatusMessage);
                    }));
                });
                saveOperation.Completed = new AsyncOperationWithProgressCompletedHandler<TranscodeFailureReason, double>(async (info, status) =>
                {
                    await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(() =>
                    {
                        try
                        {
                            var results = info.GetResults();
                            if (results != TranscodeFailureReason.None || status != AsyncStatus.Completed)
                            {
                                rootPage.NotifyUser("Saving was unsuccessful", NotifyType.ErrorMessage);
                            }
                            else
                            {
                                rootPage.NotifyUser("Trimmed clip saved to file", NotifyType.StatusMessage);
                            }
                        }
                        finally
                        {
                            // Remember to re-enable controls on both success and failure
                            EnableButtons(true);
                        }
                    }));
                });
            }
            else
            {
                rootPage.NotifyUser("User cancelled the file selection", NotifyType.StatusMessage);
                EnableButtons(true);
            }
        }

        private void EnableButtons(bool isEnabled)
        {
            chooseFile.IsEnabled = isEnabled;
            save.IsEnabled = isEnabled;
            trimClip.IsEnabled = isEnabled;
        }
    }
}
