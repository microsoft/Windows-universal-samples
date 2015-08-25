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
using Windows.ApplicationModel.Contacts;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Contact cards C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Show mini contact card", ClassType=typeof(Scenario1_Mini)},
            new Scenario() { Title="Show mini contact card with delayed information", ClassType=typeof(Scenario2_DelayMini)},
            new Scenario() { Title="Show full contact card", ClassType=typeof(Scenario3_Full)},
        };

        public Contact CreateContactFromUserInput(TextBox EmailAddress, TextBox PhoneNumber)
        {
            if (EmailAddress.Text.Length == 0 && PhoneNumber.Text.Length == 0)
            {
                NotifyUser("You must enter an email address and/or phone number.", NotifyType.ErrorMessage);
                return null;
            }

            Contact contact = new Contact();

            // Maximum length for email address is 321, enforced by XAML markup.
            if (EmailAddress.Text.Length > 0)
            {
                ContactEmail email = new ContactEmail() { Address = EmailAddress.Text };
                contact.Emails.Add(email);
            }

            // Maximum length for phone number is 50, enforced by XAML markup.
            if (PhoneNumber.Text.Length > 0)
            {
                ContactPhone phone = new ContactPhone() { Number = PhoneNumber.Text };
                contact.Phones.Add(phone);
            }

            return contact;
        }

        public static Rect GetElementRect(FrameworkElement element)
        {
            Windows.UI.Xaml.Media.GeneralTransform buttonTransform = element.TransformToVisual(null);
            Point point = buttonTransform.TransformPoint(new Point());
            return new Rect(point, new Size(element.ActualWidth, element.ActualHeight));
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
