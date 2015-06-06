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
    public sealed partial class BuildNewRss : Page
    {
        private MainPage rootPage;

        public BuildNewRss()
        {
            this.InitializeComponent();

            Scenario1Init();
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

        /// <summary>
        /// This is the click handler for the 'Scenario1BtnDefault' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Scenario1BtnDefault_Click(object sender, RoutedEventArgs e)
        {
            String rss = scenario1RssInput.Text;
            if (null != rss && "" != rss)
            {
                String xml;
                var doc = new Windows.Data.Xml.Dom.XmlDocument();
                scenario1OriginalData.Document.GetText(Windows.UI.Text.TextGetOptions.None, out xml);
                doc.LoadXml(xml);

                // create a rss CDataSection and insert into DOM tree
                var cdata = doc.CreateCDataSection(rss);
                var element = doc.GetElementsByTagName("content").Item(0);
                element.AppendChild(cdata);

                Scenario.RichEditBoxSetMsg(scenario1Result, doc.GetXml(), true);
            }
            else
            {
                Scenario.RichEditBoxSetError(scenario1Result, "Please type in RSS content in the [RSS Content] box firstly.");
            }
        }

        private async void Scenario1Init()
        {
            try
            {
                Windows.Data.Xml.Dom.XmlDocument doc = await Scenario.LoadXmlFile("buildRss", "rssTemplate.xml");
                Scenario.RichEditBoxSetMsg(scenario1OriginalData, doc.GetXml(), true);
            }
            catch (Exception exp)
            {
                Scenario.RichEditBoxSetError(scenario1Result, exp.Message);
                rootPage.NotifyUser("Exception occured while loading xml file!", NotifyType.ErrorMessage);
            }
        }
    }
}
