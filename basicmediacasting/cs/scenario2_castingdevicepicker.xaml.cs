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
using SDKTemplate;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Media;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.Media.Casting;

namespace BasicMediaCasting
{
    public sealed partial class Scenario2 : Page
    {
        private MainPage rootPage;
        private CastingDevicePicker picker;

        public Scenario2()
        {
            this.InitializeComponent();

            //Initialize our picker object
            picker = new CastingDevicePicker();

            //Set the picker to filter to video capable casting devices
            picker.Filter.SupportsVideo = true;

            //Hook up device selected event
            picker.CastingDeviceSelected += Picker_CastingDeviceSelected;
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

        private void castButton_Click(object sender, RoutedEventArgs e)
        {
            //Retrieve the location of the casting button
            GeneralTransform transform = castButton.TransformToVisual(Window.Current.Content as UIElement);
            Point pt = transform.TransformPoint(new Point(0, 0));

            //Show the picker above our casting button
            picker.Show(new Rect(pt.X, pt.Y, castButton.ActualWidth, castButton.ActualHeight), Windows.UI.Popups.Placement.Above);

            rootPage.NotifyUser("Cast Button Clicked", NotifyType.StatusMessage);
        }

        private async void Picker_CastingDeviceSelected(CastingDevicePicker sender, CastingDeviceSelectedEventArgs args)
        {
            //Casting must occur from the UI thread.  This dispatches the casting calls to the UI thread.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                //Create a casting conneciton from our selected casting device
                CastingConnection connection = args.SelectedCastingDevice.CreateCastingConnection();

                //Hook up the casting events
                connection.ErrorOccurred += Connection_ErrorOccurred;
                connection.StateChanged += Connection_StateChanged;

                //Cast the content loaded in the media element to the selected casting device
                await connection.RequestStartCastingAsync(video.GetAsCastingSource());
            });
        }

        private async void Connection_StateChanged(CastingConnection sender, object args)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser("Casting Connection State Changed: " + sender.State, NotifyType.StatusMessage);
            });
        }

        private async void Connection_ErrorOccurred(CastingConnection sender, CastingConnectionErrorOccurredEventArgs args)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser("Casting Error Occured: " + args.Message, NotifyType.ErrorMessage);
            });
        }
    }
}
