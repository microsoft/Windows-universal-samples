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
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class ShareHtml
    {
        public ShareHtml()
        {
            this.InitializeComponent();
            ShareWebView.Navigate(new Uri("http://msdn.microsoft.com"));
        }

        private void ShareWebView_NavigationCompleted(object sender, WebViewNavigationCompletedEventArgs e)
        {
            ShareWebView.Visibility = Windows.UI.Xaml.Visibility.Visible;
            BlockingRect.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            LoadingProgressRing.IsActive = false;
        }

        protected override bool GetShareContent(DataRequest request)
        {
            // Get the user's selection from the WebView. Since this is an asynchronous operation we need to acquire the deferral first.
            DataRequestDeferral deferral = request.GetDeferral();

            // Make sure to always call Complete when done with the deferral.
            try
            {
                var requestDataOperation = ShareWebView.CaptureSelectedContentToDataPackageAsync();
                requestDataOperation.Completed += (asyncInfo, status) =>
                {
                    DataPackage requestData = asyncInfo.GetResults();
                    if ((requestData != null) && (requestData.GetView().AvailableFormats.Count > 0))
                    {
                        requestData.Properties.Title = "A web snippet for you";
                        requestData.Properties.Description = "HTML selection from a WebView control"; // The description is optional.
                        requestData.Properties.ContentSourceApplicationLink = ApplicationLink;
                        requestData.Properties.ContentSourceWebLink = new Uri("http://msdn.microsoft.com");
                        request.Data = requestData;
                        deferral.Complete();
                    }
                    else
                    {
                        // FailWithDisplayText calls Complete on the deferral.
                        request.FailWithDisplayText("Make a selection in the WebView control and try again.");
                    }
                };          
            }
            catch (Exception)
            {
                deferral.Complete();
            }

            // At this point, we haven't populated the data package yet. It's done asynchronously above.
            return false;
        }
    }
}
