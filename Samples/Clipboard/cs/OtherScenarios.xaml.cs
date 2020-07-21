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
using Windows.ApplicationModel.DataTransfer;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class OtherScenarios : Page
    {
        MainPage rootPage = MainPage.Current;

        public OtherScenarios()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            RegisterClipboardContentChanged.IsChecked = rootPage.IsClipboardContentChangedEnabled;
        }

        void ShowFormatButton_Click(object sender, RoutedEventArgs e)
        {
            this.DisplayFormats();
        }

        void EmptyClipboardButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Clipboard.Clear();
                OutputText.Text = "Clipboard has been emptied.";
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Error emptying Clipboard: " + ex.Message + ". Try again", NotifyType.ErrorMessage);
            }
        }

        void RegisterClipboardContentChanged_Click(object sender, RoutedEventArgs e)
        {
            // In this sample, we register for Clipboard update notifications on the MainPage so that we can
            // navigate between scenarios and still receive updates. The sample also registers for window activated 
            // notifications since the app needs to be in the foreground to access the clipboard. Once we receive
            // a clipboard update notification, we display the new content (if the app is in the foreground). If the
            // sample is not in the foreground, we defer displaying it until it is.
            bool enable = RegisterClipboardContentChanged.IsChecked.Value;
            if (rootPage.EnableClipboardContentChangedNotifications(enable))
            {
                this.ClearOutput();

                if (enable)
                {
                    OutputText.Text = "Successfully registered for clipboard update notification.";
                }
                else
                {
                    OutputText.Text = "Successfully un-registered for clipboard update notification.";
                }
            }
        }

        void ClearOutputButton_Click(object sender, RoutedEventArgs e)
        {
            this.ClearOutput();
        }

        private void ClearOutput()
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);
            OutputText.Text = "";
        }

        private void DisplayFormats()
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);
            OutputText.Text = rootPage.BuildClipboardFormatsOutputString();
        }
    }
}
