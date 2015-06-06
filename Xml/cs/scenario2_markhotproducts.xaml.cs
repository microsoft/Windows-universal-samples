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
    public sealed partial class MarkHotProducts : Page
    {
        private MainPage rootPage;

        public MarkHotProducts()
        {
            this.InitializeComponent();

            Scenario2Init();
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
        /// This is the click handler for the 'Scenario2BtnDefault' button.
        /// This function will look up products and mark hot products.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Scenario2BtnDefault_Click(object sender, RoutedEventArgs e)
        {
            String xml;
            scenario2OriginalData.Document.GetText(Windows.UI.Text.TextGetOptions.None, out xml);

            var doc = new Windows.Data.Xml.Dom.XmlDocument();
            doc.LoadXml(xml);

            // Mark 'hot' attribute to '1' if 'sell10days' is greater than 'InStore'
            var xpath = "/products/product[Sell10day>InStore]/@hot";
            var hotAttributes = doc.SelectNodes(xpath);
            for (uint index = 0; index < hotAttributes.Length; index++)
            {
                hotAttributes.Item(index).NodeValue = "1";
            }

            Scenario.RichEditBoxSetMsg(scenario2Result, doc.GetXml(), true);
            scenario2BtnSave.IsEnabled = true;  // enable Save button
        }

        /// <summary>
        /// This is the click handler for the 'Scenario2BtnSave' button.
        /// This function is to save the new xml in which hot products are marked to a file.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void Scenario2BtnSave_Click(object sender, RoutedEventArgs e)
        {
            String xml;
            scenario2Result.Document.GetText(Windows.UI.Text.TextGetOptions.None, out xml);

            var doc = new Windows.Data.Xml.Dom.XmlDocument();
            doc.LoadXml(xml);

            // save xml to a file
            var file = await Windows.Storage.ApplicationData.Current.LocalFolder.CreateFileAsync("HotProducts.xml", Windows.Storage.CreationCollisionOption.GenerateUniqueName);
            await doc.SaveToFileAsync(file);

            Scenario.RichEditBoxSetMsg(scenario2Result, "Save to \"" + file.Path + "\" successfully.", true);

            scenario2BtnSave.IsEnabled = false;
        }

        private async void Scenario2Init()
        {
            try
            {
                Windows.Data.Xml.Dom.XmlDocument doc = await Scenario.LoadXmlFile("markHotProducts", "products.xml");
                Scenario.RichEditBoxSetMsg(scenario2OriginalData, doc.GetXml(), true);
            }
            catch (Exception exp)
            {
                Scenario.RichEditBoxSetError(scenario2Result, exp.Message);
                rootPage.NotifyUser("Exception occured while loading xml file!", NotifyType.ErrorMessage);
            }
        }
    }
}
