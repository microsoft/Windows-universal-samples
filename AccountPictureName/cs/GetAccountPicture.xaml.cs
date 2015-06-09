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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using System;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml.Media.Imaging;
using Windows.System.UserProfile;

namespace SDKTemplate
{
    public sealed partial class GetAccountPicture : Page
    {
        private MainPage rootPage;

        public GetAccountPicture()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private async void GetSmallImageButton_Click(object sender, RoutedEventArgs e)
        {
            HideImageAndVideoControls();

            if (UserInformation.NameAccessAllowed)
            {
                // The small picture returned by GetAccountPicture() is 96x96 pixels in size.
                StorageFile image = UserInformation.GetAccountPicture(AccountPictureKind.SmallImage) as StorageFile;
                if (image != null)
                {
                    rootPage.NotifyUser("Path = " + image.Path, NotifyType.StatusMessage);

                    try
                    {
                        IRandomAccessStream imageStream = await image.OpenReadAsync();
                        BitmapImage bitmapImage = new BitmapImage();
                        bitmapImage.SetSource(imageStream);
                        SmallImage.Source = bitmapImage;
                        SmallImage.Visibility = Visibility.Visible;
                    }
                    catch (Exception ex)
                    {
                        rootPage.NotifyUser("Error opening stream: " + ex.ToString(), NotifyType.ErrorMessage);
                    }
                }
                else
                {
                    rootPage.NotifyUser("Small account picture is not available.", NotifyType.StatusMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Access to account picture disabled by Privacy Setting or Group Policy.", NotifyType.ErrorMessage);
            }
        }

        private async void GetLargeImageButton_Click(object sender, RoutedEventArgs e)
        {
            HideImageAndVideoControls();

            if (UserInformation.NameAccessAllowed)
            {
                // The large picture returned by GetAccountPicture() is 448x448 pixels in size.
                StorageFile image = UserInformation.GetAccountPicture(AccountPictureKind.LargeImage) as StorageFile;
                if (image != null)
                {
                    rootPage.NotifyUser("Path = " + image.Path, NotifyType.StatusMessage);

                    try
                    {
                        IRandomAccessStream imageStream = await image.OpenReadAsync();
                        BitmapImage bitmapImage = new BitmapImage();
                        bitmapImage.SetSource(imageStream);
                        LargeImage.Source = bitmapImage;
                        LargeImage.Visibility = Visibility.Visible;
                    }
                    catch (Exception ex)
                    {
                        rootPage.NotifyUser("Error opening stream: " + ex.ToString(), NotifyType.ErrorMessage);
                    }
                }
                else
                {
                    rootPage.NotifyUser("Large account picture is not available.", NotifyType.StatusMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Access to account picture disabled by Privacy Setting or Group Policy.", NotifyType.ErrorMessage);
            }
        }

        private async void GetVideoButton_Click(object sender, RoutedEventArgs e)
        {
            HideImageAndVideoControls();

            if (UserInformation.NameAccessAllowed)
            {
                // The video returned from getAccountPicture is 448x448 pixels in size.
                StorageFile video = UserInformation.GetAccountPicture(AccountPictureKind.Video) as StorageFile;
                if (video != null)
                {
                    rootPage.NotifyUser("Path: " + video.Path, NotifyType.StatusMessage);

                    try
                    {
                        IRandomAccessStream videoStream = await video.OpenAsync(FileAccessMode.Read);
                        MediaPlayer.SetSource(videoStream, "video/mp4");
                        MediaPlayer.Visibility = Visibility.Visible;
                    }
                    catch (Exception ex)
                    {
                        rootPage.NotifyUser("Error opening stream: " + ex.ToString(), NotifyType.ErrorMessage);
                    }
                }
                else
                {
                    rootPage.NotifyUser("Video is not available.", NotifyType.StatusMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Access to account picture disabled by Privacy Setting or Group Policy.", NotifyType.ErrorMessage);
            }
        }

        private void HideImageAndVideoControls()
        {
            SmallImage.Visibility = Visibility.Collapsed;
            LargeImage.Visibility = Visibility.Collapsed;
            MediaPlayer.Visibility = Visibility.Collapsed;
        }

    }
}
