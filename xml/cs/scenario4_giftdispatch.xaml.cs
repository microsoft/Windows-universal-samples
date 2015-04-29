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
using System.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Xml
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class GiftDispatch : Page
    {
        private MainPage rootPage;

        public GiftDispatch()
        {
            this.InitializeComponent();

            Scenario4Init();
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

        async void Scenario4Init()
        {
            try
            {
                Windows.Data.Xml.Dom.XmlDocument doc = await Scenario.LoadXmlFile("giftDispatch", "employees.xml");
                Scenario.RichEditBoxSetMsg(scenario4OriginalData, doc.GetXml(), true);
            }
            catch (Exception exp)
            {
                Scenario.RichEditBoxSetError(scenario4Result, exp.Message);
                rootPage.NotifyUser("Exception occured while loading xml file!", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for the 'Scenario4BtnDefault' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Scenario4BtnDefault_Click(object sender, RoutedEventArgs e)
        {
            String xml;
            scenario4OriginalData.Document.GetText(Windows.UI.Text.TextGetOptions.None, out xml);

            var doc = new Windows.Data.Xml.Dom.XmlDocument();
            doc.LoadXml(xml);

            var thisYear = 2012;    // Here we don't use DateTime.Now.Year to get the current year so that all gifts can be delivered.
            var previousOneYear = thisYear - 1;
            var previousFiveYear = thisYear - 5;
            var previousTenYear = thisYear - 10;

            var xpathArray = new String[3];
            // select >= 1 year and < 5 years
            xpathArray[0] = "descendant::employee[startyear <= " + previousOneYear + " and startyear > " + previousFiveYear + "]";
            // select >= 5 years and < 10 years
            xpathArray[1] = "descendant::employee[startyear <= " + previousFiveYear + " and startyear > " + previousTenYear + "]";
            // select >= 10 years
            xpathArray[2] = "descendant::employee[startyear <= " + previousTenYear + "]";

            var Gifts = new String[3] { "Gift Card", "XBOX", "Windows Phone" };

            var output = new StringBuilder();
            uint i = 0;
            foreach (var xpath in xpathArray)
            {
                var employees = doc.SelectNodes(xpath);
                foreach (var emplopyee in employees)
                {
                    var employeeName = emplopyee.SelectSingleNode("descendant::name");
                    var department = emplopyee.SelectSingleNode("descendant::department");

                    output.AppendFormat("[{0}]/[{1}]/[{2}]\n", employeeName.FirstChild.NodeValue, department.FirstChild.NodeValue, Gifts[(i++) % 3]);
                }
            }

            Scenario.RichEditBoxSetMsg(scenario4Result, output.ToString(), true);
        }
    }
}
