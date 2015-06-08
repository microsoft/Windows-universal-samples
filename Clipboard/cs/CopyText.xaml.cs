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
using System.Collections.Generic;
using Windows.ApplicationModel.DataTransfer;
using Windows.Data.Html;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class CopyText : Page
    {
        MainPage rootPage = MainPage.Current;

        public CopyText()
        {
            this.InitializeComponent();
            this.Init();
        }

        #region Scenario Specific Code

        void Init()
        {
            CopyButton.Click += new RoutedEventHandler(CopyButton_Click);
            PasteButton.Click += new RoutedEventHandler(PasteButton_Click);
            Description.NavigateToString(this.htmlFragment);
        }

        #endregion

        #region Button click

        void CopyButton_Click(object sender, RoutedEventArgs e)
        {
            OutputText.Text = "";
            OutputResourceMapKeys.Text = "";
            OutputHtml.NavigateToString("<HTML></HTML>");

            // Set the content to DataPackage as html format
            string htmlFormat = HtmlFormatHelper.CreateHtmlFormat(this.htmlFragment);
            var dataPackage = new DataPackage();
            dataPackage.SetHtmlFormat(htmlFormat);

            // Set the content to DataPackage as (plain) text format
            string plainText = HtmlUtilities.ConvertToText(this.htmlFragment);
            dataPackage.SetText(plainText);

            // Populate resourceMap with StreamReference objects corresponding to local image files embedded in HTML
            var imgUri = new Uri(imgSrc);
            var imgRef = RandomAccessStreamReference.CreateFromUri(imgUri);
            dataPackage.ResourceMap[imgSrc] = imgRef;

            try
            {
                // Set the DataPackage to clipboard.
                Windows.ApplicationModel.DataTransfer.Clipboard.SetContent(dataPackage);
                OutputText.Text = "Text and HTML formats have been copied to clipboard. ";
            }
            catch (Exception ex)
            {
                // Copying data to Clipboard can potentially fail - for example, if another application is holding Clipboard open
                rootPage.NotifyUser("Error copying content to Clipboard: " + ex.Message + ". Try again", NotifyType.ErrorMessage);
            }
        }

        async void PasteButton_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);
            OutputText.Text = "Content in the clipboard: ";
            OutputResourceMapKeys.Text = "";
            OutputHtml.NavigateToString("<HTML></HTML>");

            var dataPackageView = Windows.ApplicationModel.DataTransfer.Clipboard.GetContent();
            if (dataPackageView.Contains(StandardDataFormats.Text))
            {
                try
                {
                    var text = await dataPackageView.GetTextAsync();
                    OutputText.Text = "Text: " + Environment.NewLine + text;
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser("Error retrieving Text format from Clipboard: " + ex.Message, NotifyType.ErrorMessage);
                }
            }
            else
            {
                OutputText.Text = "Text: " + Environment.NewLine + "Text format is not available in clipboard";
            }



            if (dataPackageView.Contains(StandardDataFormats.Html))
            {
                this.DisplayResourceMapAsync(dataPackageView);

                string htmlFormat = null;
                try
                {
                    htmlFormat = await dataPackageView.GetHtmlFormatAsync();
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser("Error retrieving HTML format from Clipboard: " + ex.Message, NotifyType.ErrorMessage);
                }

                if (htmlFormat != null)
                {
                    string htmlFragment = HtmlFormatHelper.GetStaticFragment(htmlFormat);
                    OutputHtml.NavigateToString("HTML:<br/ > " + htmlFragment);
                }
            }
            else
            {
                OutputHtml.NavigateToString("HTML:<br/ > HTML format is not available in clipboard");
            }
        }

        #endregion

        #region private member variables

        private string htmlFragment = "Use clipboard to copy and paste text in different formats, including plain text, and formatted text (HTML). <br />"
                                    + " To <b>copy</b>, add text formats to a <i>DataPackage</i>, and then place <i>DataPackage</i> to Clipboard.<br /> "
                                    + "To <b>paste</b>, get <i>DataPackageView</i> from clipboard, and retrieve the content of the desired text format from it.<br />"
                                    + "To handle local image files in the formatted text (such as the one below), use ResourceMap collection."
                                    + "<br /><img id=\"scenario1LocalImage\" src=\""
                                    + imgSrc
                                    + "\" /><br />"
                                    + "<i>DataPackage</i> class is also used to share or send content between applications. <br />"
                                    + "See the Share SDK sample for more information.";

        private const string imgSrc = "ms-appx-web:///assets/windows-sdk.png";

        #endregion

        #region helper functions

        // Note: this sample is not trying to resolve and render the HTML using resource map.
        // Please refer to the Clipboard JavaScript sample for an example of how to use resource map
        // for local images display within an HTML format. This sample will only demonstrate how to
        // get a resource map object and extract its key values
        async void DisplayResourceMapAsync(DataPackageView dataPackageView)
        {
            OutputResourceMapKeys.Text = Environment.NewLine + "Resource map: ";
            IReadOnlyDictionary<string, RandomAccessStreamReference> resMap = null;
            try
            {
                resMap = await dataPackageView.GetResourceMapAsync();
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Error retrieving Resource map from Clipboard: " + ex.Message, NotifyType.ErrorMessage);
            }

            if (resMap != null)
            {
                if (resMap.Count > 0)
                {
                    foreach (var item in resMap)
                    {
                        OutputResourceMapKeys.Text += Environment.NewLine + "Key: " + item.Key;
                    }
                }
                else
                {
                    OutputResourceMapKeys.Text += Environment.NewLine + "Resource map is empty";
                }
            }
        }

        #endregion
    }
}
