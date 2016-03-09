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

using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using System;

namespace SDKTemplate
{
    public sealed partial class Scenario3_InvokeScript : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario3_InvokeScript()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            HtmlBox.Text = await MainPage.LoadStringFromPackageFileAsync("script_example.html");
        }

        private void LoadHtml()
        {
            // Grab the HTML from the text box and load it into the WebView.
            WebViewControl.NavigateToString(HtmlBox.Text);
        }

        private async void InvokeScript()
        {
            // Invoke the javascript function called 'doSomething' that is loaded into the WebView.
            try
            {
                string result = await WebViewControl.InvokeScriptAsync("doSomething", null);
                rootPage.NotifyUser($"Script returned \"{result}\"", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                string errorText = null;
                switch (ex.HResult)
                {
                    case unchecked((int)0x80020006):
                        errorText = "There is no function called doSomething";
                        break;
                    case unchecked((int)0x80020101):
                        errorText = "A JavaScript error or exception occured while executing the function doSomething";
                        break;

                    case unchecked((int)0x800a138a):
                        errorText = "doSomething is not a function";
                        break;
                    default:
                        // Some other error occurred.
                        errorText = ex.Message;
                        break;
                }
                rootPage.NotifyUser(errorText, NotifyType.ErrorMessage);
            }
        }
    }
}
