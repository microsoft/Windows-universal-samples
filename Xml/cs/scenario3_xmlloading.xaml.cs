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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Xml
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class XmlLoading : Page
    {
        private MainPage rootPage;

        public XmlLoading()
        {
            this.InitializeComponent();

            Scenario3Init();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        async void Scenario3Init()
        {
            try
            {
                Windows.Data.Xml.Dom.XmlDocument doc = await Scenario.LoadXmlFile("loadExternaldtd", "xmlWithExternaldtd.xml");
                Scenario.RichEditBoxSetMsg(scenario3OriginalData, doc.GetXml(), true);
            }
            catch (Exception exp)
            {
                Scenario.RichEditBoxSetError(scenario3Result, exp.Message);
                rootPage.NotifyUser("Exception occured while loading xml file!", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for the 'scenario3BtnBuffer' button.  You would replace this with your own handler
        /// if you have a button or buttons on this page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void Scenario3BtnBuffer_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                // Get load settings
                var loadSettings = new Windows.Data.Xml.Dom.XmlLoadSettings();
                if (true == scenario3RB1.IsChecked)
                {
                    loadSettings.ProhibitDtd = true;        // DTD is prohibited
                    loadSettings.ResolveExternals = false;  // Disable the resolve to external definitions such as external DTD
                }
                else if (true == scenario3RB2.IsChecked)
                {
                    loadSettings.ProhibitDtd = false;       // DTD is not prohibited
                    loadSettings.ResolveExternals = false;  // Disable the resolve to external definitions such as external DTD
                }
                else if (true == scenario3RB3.IsChecked)
                {
                    loadSettings.ProhibitDtd = false;       // DTD is not prohibited
                    loadSettings.ResolveExternals = true;   // Enable the resolve to external definitions such as external DTD
                }

                String xml;

                scenario3OriginalData.Document.GetText(Windows.UI.Text.TextGetOptions.None, out xml);

                // Set external dtd file path
                if (loadSettings.ResolveExternals == true && loadSettings.ProhibitDtd == false)
                {
                    Windows.Storage.StorageFolder storageFolder = await Windows.ApplicationModel.Package.Current.InstalledLocation.GetFolderAsync("loadExternaldtd");
                    String dtdPath = storageFolder.Path + "\\dtd.txt";
                    xml = xml.Replace("dtd.txt", dtdPath);
                }

                var dataWriter = new Windows.Storage.Streams.DataWriter();

                dataWriter.WriteString(xml);

                Windows.Storage.Streams.IBuffer ibuffer = dataWriter.DetachBuffer();

                var doc = new Windows.Data.Xml.Dom.XmlDocument();

                doc.LoadXmlFromBuffer(ibuffer, loadSettings);

                Scenario.RichEditBoxSetMsg(scenario3Result, doc.GetXml(), true);
            }
            catch (Exception)
            {
                // After loadSettings.ProhibitDtd is set to true, the exception is expected as the sample XML contains DTD
                Scenario.RichEditBoxSetError(scenario3Result, "Error: DTD is prohibited");
            }
        }

        /// <summary>
        /// This is the click handler for the 'Scenario3BtnFile' button.  You would replace this with your own handler
        /// if you have a button or buttons on this page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void Scenario3BtnFile_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                // get load settings
                var loadSettings = new Windows.Data.Xml.Dom.XmlLoadSettings();
                if (true == scenario3RB1.IsChecked)
                {
                    loadSettings.ProhibitDtd = true;        // DTD is prohibited
                    loadSettings.ResolveExternals = false;  // Disable the resolve to external definitions such as external DTD
                }
                else if (true == scenario3RB2.IsChecked)
                {
                    loadSettings.ProhibitDtd = false;       // DTD is not prohibited
                    loadSettings.ResolveExternals = false;  // Disable the resolve to external definitions such as external DTD
                }
                else if (true == scenario3RB3.IsChecked)
                {
                    loadSettings.ProhibitDtd = false;       // DTD is not prohibited
                    loadSettings.ResolveExternals = true;   // Enable the resolve to external definitions such as external DTD
                }

                var folder = await Windows.ApplicationModel.Package.Current.InstalledLocation.GetFolderAsync("loadExternaldtd");
                var file = await folder.GetFileAsync("xmlWithExternaldtd.xml");

                var doc = await Windows.Data.Xml.Dom.XmlDocument.LoadFromFileAsync(file, loadSettings);

                Scenario.RichEditBoxSetMsg(scenario3Result, doc.GetXml(), true);
            }
            catch (Exception)
            {
                // After loadSettings.ProhibitDtd is set to true, the exception is expected as the sample XML contains DTD
                Scenario.RichEditBoxSetError(scenario3Result, "Error: DTD is prohibited");
            }
        }
        async void LaunchUri(object sender, RoutedEventArgs e)
        {
            await Windows.System.Launcher.LaunchUriAsync(new Uri(((HyperlinkButton)sender).Tag.ToString()));
        }
    }
}
