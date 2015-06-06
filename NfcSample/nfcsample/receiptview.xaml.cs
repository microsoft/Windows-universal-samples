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

using SDKTemplate;
using System;
using Windows.Storage;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace NfcSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ReceiptView : Page
    {
        public ReceiptView()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            lblIcon.Text = "";
            lblStatus.Text = System.DateTime.Now.ToString() + "\r\n";
            lblResult.Text = "";
            if (e.Parameter is string)
            {
                var args = ((string)e.Parameter).Split('^');

                if (args.Length >= 2)
                {
                    lblResult.Text = args[1].ToUpper();
                    if (args[1] == "Failed")
                    {
                        // X
                        lblIcon.Text = "\uE10A";
                        lblIcon.Foreground = new SolidColorBrush(Windows.UI.Colors.Red);
                    }
                    else if (args[1] == "Complete")
                    {
                        // Checkmark
                        lblIcon.Text = "\uE001";
                        lblIcon.Foreground = new SolidColorBrush(Windows.UI.Colors.Green);
                    }
                    else if (args[1] == "Denied")
                    {
                        // !
                        lblIcon.Text = "\uE1DE";
                        lblIcon.Foreground = new SolidColorBrush(Windows.UI.Colors.Orange);
                    }

                    if (args.Length >= 3)
                    {
                        lblStatus.Text += args[2];
                    }
                }
            }
        }

        private void btnMainMenu_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            ((Frame)Windows.UI.Xaml.Window.Current.Content).Navigate(typeof(MainPage));
        }

        private async void btnDebugLog_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            var debugLogFile = (StorageFile)(await Windows.Storage.ApplicationData.Current.LocalFolder.TryGetItemAsync("DebugLog.txt"));
            if (debugLogFile != null)
            {
                if (txtDebugLog.Visibility == Windows.UI.Xaml.Visibility.Visible)
                {
                    // Log was already displayed, clear it
                    await debugLogFile.DeleteAsync();
                    txtDebugLog.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                    btnDebugLog.Content = "Debug Log";
                }
                else
                {
                    // Display the log
                    txtDebugLog.Text = await FileIO.ReadTextAsync(debugLogFile);
                    txtDebugLog.Visibility = Windows.UI.Xaml.Visibility.Visible;
                    btnDebugLog.Content = "Clear Debug Log";
                }
            }
        }
    }
}
