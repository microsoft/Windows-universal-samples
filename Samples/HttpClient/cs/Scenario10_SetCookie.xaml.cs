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
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;
using Windows.Web.Http.Filters;

namespace SDKTemplate
{
    public sealed partial class Scenario10_SetCookie : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario10_SetCookie()
        {
            this.InitializeComponent();
        }

        private void SetCookie_Click(object sender, RoutedEventArgs e)
        {
            if (String.IsNullOrEmpty(NameField.Text) || String.IsNullOrEmpty(DomainField.Text) || String.IsNullOrEmpty(PathField.Text))
            {
                rootPage.NotifyUser("Invalid name, domain, or path value.", NotifyType.ErrorMessage);
                return;
            }

            try
            {
                HttpCookie cookie = new HttpCookie(NameField.Text, DomainField.Text, PathField.Text);
                cookie.Value = ValueField.Text;

                if (NullCheckBox.IsChecked.Value)
                {
                    cookie.Expires = null;
                }
                else
                {
                    cookie.Expires = ExpiresDatePicker.Date + ExpiresTimePicker.Time;
                }

                cookie.Secure = SecureToggle.IsOn;
                cookie.HttpOnly = HttpOnlyToggle.IsOn;

                HttpBaseProtocolFilter filter = new HttpBaseProtocolFilter();
                bool replaced = filter.CookieManager.SetCookie(cookie, false);

                if (replaced)
                {
                    rootPage.NotifyUser("Cookie replaced.", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Cookie set.", NotifyType.StatusMessage);
                }
            }
            catch (ArgumentException ex)
            {
                rootPage.NotifyUser(ex.Message, NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Error: " + ex.Message, NotifyType.ErrorMessage);
            }
        }
    }
}
