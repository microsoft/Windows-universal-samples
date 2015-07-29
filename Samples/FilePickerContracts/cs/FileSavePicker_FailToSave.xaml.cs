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
    public sealed partial class FileSavePicker_FailToSave : Page
    {
        FileSavePickerUI fileSavePickerUI = FileSavePickerPage.Current.fileSavePickerUI;
        CoreDispatcher dispatcher = Window.Current.Dispatcher;

        public FileSavePicker_FailToSave()
        {
            this.InitializeComponent();
        }

        private async void OnTargetFileRequested(FileSavePickerUI sender, TargetFileRequestedEventArgs e)
        {
            // This scenario demonstrates how the app can go about handling the TargetFileRequested event on the UI thread, from
            // which the app can manipulate the UI, show error dialogs, etc.

            // Requesting a deferral allows the app to return from this event handler and complete the request at a later time.
            // In this case, the deferral is required as the app intends on handling the TargetFileRequested event on the UI thread.
            // Note that the deferral can be requested more than once but calling Complete on the deferral a single time will complete
            // original TargetFileRequested event.
            var deferral = e.Request.GetDeferral();

            await dispatcher.RunAsync(CoreDispatcherPriority.Normal, async () =>
            {
                // This method will be called on the app's UI thread, which allows for actions like manipulating
                // the UI or showing error dialogs

                // Display a dialog indicating to the user that a corrective action needs to occur
                var errorDialog = new Windows.UI.Popups.MessageDialog("If the app needs the user to correct a problem before the app can save the file, the app can use a message like this to tell the user about the problem and how to correct it.");
                await errorDialog.ShowAsync();

                // Set the targetFile property to null and complete the deferral to indicate failure once the user has closed the
                // dialog.  This will allow the user to take any neccessary corrective action and click the Save button once again.
                e.Request.TargetFile = null;
                deferral.Complete();
            });
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
