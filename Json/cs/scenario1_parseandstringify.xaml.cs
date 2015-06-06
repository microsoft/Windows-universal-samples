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
using System.Diagnostics;
using Windows.Data.Json;

namespace Json
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class S1_ParseAndStringify : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public S1_ParseAndStringify()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            JsonInput.Text = "{\r\n" +
                "  \"id\": \"1146217767\",\r\n" +
                "  \"phone\": null,\r\n" +
                "  \"name\": \"Satya Nadella\",\r\n" +
                "  \"education\": [\r\n" +
                "    {\r\n" +
                "      \"school\": {\r\n" +
                "        \"id\": \"204165836287254\",\r\n" +
                "        \"name\": \"Contoso High School\"\r\n" +
                "      },\r\n" +
                "      \"type\": \"High School\"\r\n" +
                "    },\r\n" +
                "    {\r\n" +
                "      \"school\": {\r\n" +
                "        \"id\": \"116138758396662\",\r\n" +
                "        \"name\": \"Contoso University\"\r\n" +
                "      },\r\n" +
                "      \"type\": \"College\"\r\n" +
                "    }\r\n" +
                "  ],\r\n" +
                "  \"timezone\": -8,\r\n" +
                "  \"verified\": true\r\n" +
                "}";

            rootPage.DataContext = new User();
        }

        private void Parse_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                rootPage.DataContext = new User(JsonInput.Text);
                rootPage.NotifyUser("JSON string parsed successfully.", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                if (!IsExceptionHandled(ex))
                {
                    throw ex;
                }
            }
        }

        private void Stringify_Click(object sender, RoutedEventArgs e)
        {
            JsonInput.Text = "";

            User user = rootPage.DataContext as User;
            Debug.Assert(user != null);

            JsonInput.Text = user.Stringify();
            rootPage.NotifyUser("JSON object serialized to string successfully.", NotifyType.StatusMessage);
        }

        private void Add_Click(object sender, RoutedEventArgs e)
        {
            User user = rootPage.DataContext as User;
            Debug.Assert(user != null);
            user.Education.Add(new School());
            rootPage.NotifyUser("New row added.", NotifyType.StatusMessage);
        }

        private bool IsExceptionHandled(Exception ex)
        {
            JsonErrorStatus error = JsonError.GetJsonStatus(ex.HResult);
            if (error == JsonErrorStatus.Unknown)
            {
                return false;
            }

            rootPage.NotifyUser(error + ": " + ex.Message, NotifyType.ErrorMessage);
            return true;
        }
    }
}
