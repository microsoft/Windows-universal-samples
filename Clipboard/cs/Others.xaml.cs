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
        private static bool registerContentChanged;

        public OtherScenarios()
        {
            this.InitializeComponent();
            this.Init();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            RegisterClipboardContentChanged.IsChecked = OtherScenarios.registerContentChanged;
        }

        #region Scenario Specific Code

        void Init()
        {
            ShowFormatButton.Click += new RoutedEventHandler(ShowFormatButton_Click);
            EmptyClipboardButton.Click += new RoutedEventHandler(EmptyClipboardButton_Click);
            RegisterClipboardContentChanged.Click += new RoutedEventHandler(RegisterClipboardContentChanged_Click);
            ClearOutputButton.Click += new RoutedEventHandler(ClearOutputButton_Click);
        }

        #endregion

        #region Button Click

        void ShowFormatButton_Click(object sender, RoutedEventArgs e)
        {
            this.DisplayFormats();
        }

        void EmptyClipboardButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Windows.ApplicationModel.DataTransfer.Clipboard.Clear();
                OutputText.Text = "Clipboard has been emptied.";
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Error emptying Clipboard: " + ex.Message + ". Try again", NotifyType.ErrorMessage);
            }
        }

        void RegisterClipboardContentChanged_Click(object sender, RoutedEventArgs e)
        {
            if (OtherScenarios.registerContentChanged != RegisterClipboardContentChanged.IsChecked.Value)
            {
                this.ClearOutput();
                OtherScenarios.registerContentChanged = RegisterClipboardContentChanged.IsChecked.Value;

                // In this sample, we register for Clipboard update notifications on the MainPage so that we can
                // navigate between scenarios and still receive updates. The sample also registers for window activated 
                // notifications since the app needs to be in the foreground to access the clipboard. Once we receive
                // a clipboard update notification, we display the new content (if the app is in the foreground). If the
                // sample is not in the foreground, we defer displaying it until it is.
                rootPage.EnableClipboardContentChangedNotifications(OtherScenarios.registerContentChanged);
                if (OtherScenarios.registerContentChanged)
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

        #endregion

        #region Private helper methods

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

        #endregion
    }
}
