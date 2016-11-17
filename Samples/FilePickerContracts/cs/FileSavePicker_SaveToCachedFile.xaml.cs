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
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Storage;
using Windows.Storage.Pickers.Provider;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Storage.Provider;
using Windows.ApplicationModel.Background;

namespace FilePickerContracts
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class FileSavePicker_SaveToCachedFile : Page
    {
        FileSavePickerUI fileSavePickerUI = FileSavePickerPage.Current.fileSavePickerUI;
        bool simulateUpdateConflict = false;

        public FileSavePicker_SaveToCachedFile()
        {
            this.InitializeComponent();
        }

        private async void OnTargetFileRequested(FileSavePickerUI sender, TargetFileRequestedEventArgs e)
        {
            // This scenario demonstrates how to handle the TargetFileRequested event on the background thread on which it was raised

            // Requesting a deferral allows the app to call another asynchronous method and complete the request at a later time
            var deferral = e.Request.GetDeferral();
            StorageFile file;
            // If the checkbox is checked then the requested file name will be ConflictingFile.txt instead of what was sent to us in sender.name.
            // If background task sees that ConflictingFile is in the name of the file it sets the returned status to FileUpdateStatus.UserInputNeeded.
            // This will cause a prompt for the user to open the app to fix the conflict.
            if (simulateUpdateConflict)
            {
                file = await ApplicationData.Current.LocalFolder.CreateFileAsync("ConflictingFile.txt", CreationCollisionOption.ReplaceExisting);
            }
            else
            {
                file = await ApplicationData.Current.LocalFolder.CreateFileAsync(sender.FileName, CreationCollisionOption.ReplaceExisting);
            }
            CachedFileUpdater.SetUpdateInformation(file, "CachedFile", ReadActivationMode.NotNeeded, WriteActivationMode.AfterWrite, CachedFileOptions.RequireUpdateOnAccess);
            e.Request.TargetFile = file;

            // Complete the deferral to let the Picker know the request is finished
            deferral.Complete();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            await RegisterBackgroundTaskAsync();
            fileSavePickerUI.TargetFileRequested += new TypedEventHandler<FileSavePickerUI, TargetFileRequestedEventArgs>(OnTargetFileRequested);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            fileSavePickerUI.TargetFileRequested -= new TypedEventHandler<FileSavePickerUI, TargetFileRequestedEventArgs>(OnTargetFileRequested);
        }

        private async Task RegisterBackgroundTaskAsync()
        {
            await BackgroundExecutionManager.RequestAccessAsync();

            string backgroundTaskName = "CachedFileUpdaterTask";
            string backgroundTaskEntryPoint = typeof(FilePickerContractsTasks.UpdateFileTask).FullName;

            // Do nothing if the task is already registered.
            foreach (var task in BackgroundTaskRegistration.AllTasks)
            {
                if (task.Value.Name == backgroundTaskName)
                {
                    // It is already registered. Don't register it again.
                    return;
                }
            }

            // Register the background task.
            var builder = new BackgroundTaskBuilder();
            builder.Name = backgroundTaskName;
            builder.TaskEntryPoint = backgroundTaskEntryPoint;
            builder.SetTrigger(new CachedFileUpdaterTrigger());
            try
            {
                builder.Register();
            }
            catch (Exception ex) when ((uint)ex.HResult == 0x80040154)
            {
                // REGDB_E_CLASSNOTREG exception is raised if the platform does not
                // support this trigger.
            }
        }

        private void Simulate_Update_Collision_Click(object sender, RoutedEventArgs e)
        {
            simulateUpdateConflict = SimulateUpdateCollisionCheckBox.IsChecked.HasValue && (bool)SimulateUpdateCollisionCheckBox.IsChecked;
        }
    }
}
