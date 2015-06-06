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
using Windows.Web.Http.Filters;
using Windows.Web.Http;

namespace SDKSample.HttpClientSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario10 : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario10()
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
        }

        private void SetCookie_Click(object sender, RoutedEventArgs e)
        {
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
