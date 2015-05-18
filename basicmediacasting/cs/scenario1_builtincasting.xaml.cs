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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;

namespace BasicMediaCasting
{
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage;

        public Scenario1()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private async void loadButton_Click(object sender, RoutedEventArgs e)
        {
            //Create a new picker
            FileOpenPicker filePicker = new FileOpenPicker();

            //Add filetype filters.  In this case wmv and mp4.
            filePicker.FileTypeFilter.Add(".wmv");
            filePicker.FileTypeFilter.Add(".mp4");

            //Set picker start location to the video library
            filePicker.SuggestedStartLocation = PickerLocationId.VideosLibrary;

            //Retrieve file from picker
            StorageFile file = await filePicker.PickSingleFileAsync();

            //If we got a file, load it into the media lement
            if (file != null)
            {
                IRandomAccessStream stream = await file.OpenAsync(FileAccessMode.Read);
                video.SetSource(stream, file.ContentType);
                rootPage.NotifyUser("Content Selected", NotifyType.StatusMessage);
            }
        }
    }
}
