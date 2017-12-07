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
using Windows.UI.Xaml.Documents;

namespace SDKTemplate
{
    public sealed partial class Scenario4_ScriptNotify : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario4_ScriptNotify()
        {
            this.InitializeComponent();
            WebViewControl.Navigate(new Uri("ms-appx-web:///html/scriptNotify_example.html"));
        }

        void WebViewControl_ScriptNotify(object sender, NotifyEventArgs e)
        {
            // Be sure to verify the source of the message when performing actions with the data.
            // As webview can be navigated, you need to check that the message is coming from a page/code
            // that you trust.
            rootPage.NotifyUser($"Event received from {e.CallingUri}: \"{e.Value}\"", NotifyType.StatusMessage);
        }

        private void MoreLess_Click(Hyperlink sender, HyperlinkClickEventArgs args)
        {
            var hyperlink = (Hyperlink)sender;
            if (MoreInformationText.Visibility == Visibility.Visible)
            {
                MoreInformationText.Visibility = Visibility.Collapsed;
                MoreLessText.Text = "Show more information";
            }
            else
            {
                MoreInformationText.Visibility = Visibility.Visible;
                MoreLessText.Text = "Show less information";
            }
            rootPage.NotifyUser("", NotifyType.StatusMessage);
        }
    }
}
