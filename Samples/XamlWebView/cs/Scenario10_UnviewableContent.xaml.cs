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
using SDKTemplate;
using System;
using Windows.UI.ViewManagement;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario10_UnviewableContent : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario10_UnviewableContent()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            WebViewControl.Navigate(new Uri("https://www.bing.com/search?q=site%3Amicrosoft.com+filetype%3Apdf"));
        }

         private async void WebViewControl_UnviewableContentIdentified(WebView sender, WebViewUnviewableContentIdentifiedEventArgs args)
        {
            if (args.Uri.AbsolutePath.EndsWith(".pdf"))
            {
                if (await Windows.System.Launcher.LaunchUriAsync(args.Uri))
                {
                    rootPage.NotifyUser($"Launched {args.Uri} with system launcher", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser($"Failed to launch {args.Uri}", NotifyType.ErrorMessage);
                }
            } else
            {
                rootPage.NotifyUser($"Blocked unknown file type of {args.Uri}", NotifyType.ErrorMessage);
            }
        }
    }
}
