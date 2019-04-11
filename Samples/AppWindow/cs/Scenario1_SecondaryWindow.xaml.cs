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
using Windows.UI.WindowManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Hosting;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_SecondaryWindow : Page
    {
        private AppWindow appWindow;
        private Frame appWindowFrame = new Frame();

        public Scenario1_SecondaryWindow()
        {
            InitializeComponent();
        }

        protected override async void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (appWindow != null)
            {
                await appWindow.CloseAsync();
            }
        }

        private async void Button_Click(object sender, RoutedEventArgs e)
        {
            showWindowBtn.IsEnabled = false;

            // Only ever create one window. If the AppWindow already exists call TryShow on it to bring it to foreground.
            if (appWindow == null)
            {
                // Create a new window
                appWindow = await AppWindow.TryCreateAsync();
                // Make sure we release the reference to this window, and release XAML resources, when it's closed
                appWindow.Closed += delegate { appWindow = null; appWindowFrame.Content = null; };
                // Navigate the frame to the page we want to show in the new window
                appWindowFrame.Navigate(typeof(SecondaryAppWindowPage));
                // Attach the XAML content to our window
                ElementCompositionPreview.SetAppWindowContent(appWindow, appWindowFrame);
            }

            // Now show the window
            await appWindow.TryShowAsync();

            showWindowBtn.IsEnabled = true;
        }
    }
}