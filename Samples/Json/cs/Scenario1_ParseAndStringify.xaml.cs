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
using Windows.Data.Json;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario1_ParseAndStringify : Page
    {
        MainPage rootPage = MainPage.Current;

        User CurrentUser
        {
            get => (User)DataContext;
            set => DataContext = value;
        }

        public Scenario1_ParseAndStringify()
        {
            this.InitializeComponent();
            CurrentUser = new User(JsonInput.Text);
        }

        private void Parse_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                CurrentUser = new User(JsonInput.Text);
                rootPage.NotifyUser("JSON string parsed successfully.", NotifyType.StatusMessage);
            }
            catch (Exception ex) when (IsExceptionHandled(ex))
            {
            }
        }

        private void Stringify_Click(object sender, RoutedEventArgs e)
        {
            JsonInput.Text = "";

            JsonInput.Text = CurrentUser.Stringify();
            rootPage.NotifyUser("JSON object serialized to string successfully.", NotifyType.StatusMessage);
        }

        private void Add_Click(object sender, RoutedEventArgs e)
        {
            CurrentUser.Education.Add(new School());
            rootPage.NotifyUser("New row added.", NotifyType.StatusMessage);
        }

        private void DeleteSchool_Click(object sender, RoutedEventArgs e)
        {
            var school = (School)((FrameworkElement)sender).DataContext;
            CurrentUser.Education.Remove(school);
        }

        private bool IsExceptionHandled(Exception ex)
        {
            if (ex.HResult == unchecked((int)0x8000000E)) // E_ILLEGAL_METHOD_CALL
            {
                rootPage.NotifyUser("JSON did not match expected schema: " + ex.Message, NotifyType.ErrorMessage);
                return true;
            }

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
