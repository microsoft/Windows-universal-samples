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
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.System.UserProfile;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class SetLockScreen : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public SetLockScreen()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }

        private async void PickAndSetButton_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);
            FileOpenPicker imagePicker = new FileOpenPicker
            {
                ViewMode = PickerViewMode.Thumbnail,
                SuggestedStartLocation = PickerLocationId.PicturesLibrary,
                FileTypeFilter = { ".jpg", ".jpeg", ".png", ".bmp" }
            };

            StorageFile imageFile = await imagePicker.PickSingleFileAsync();
            if (imageFile != null)
            {
                try
                {
                    // Application now has access to the picked file, setting image to lockscreen.  This will fail if the file is an invalid format.
                    await LockScreen.SetImageFileAsync(imageFile);

                    // Retrieve the lock screen image that was set
                    IRandomAccessStream imageStream = LockScreen.GetImageStream();
                    if (imageStream != null)
                    {
                        BitmapImage lockScreen = new BitmapImage();
                        lockScreen.SetSource(imageStream);
                        LockScreenImage.Source = lockScreen;
                        LockScreenImage.Visibility = Visibility.Visible;
                    }
                    else
                    {
                        LockScreenImage.Visibility = Visibility.Collapsed;
                        rootPage.NotifyUser("Setting the lock screen image failed.  Make sure your copy of Windows is activated.", NotifyType.StatusMessage);
                    }
                }
                catch (Exception)
                {
                    LockScreenImage.Visibility = Visibility.Collapsed;
                    rootPage.NotifyUser("Invalid image selected or error opening stream.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                LockScreenImage.Visibility = Visibility.Collapsed;
                rootPage.NotifyUser("No file was selected using the picker.", NotifyType.StatusMessage);
            }
        }
    }
}
