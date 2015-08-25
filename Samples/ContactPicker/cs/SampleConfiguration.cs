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
using System.Text;
using Windows.ApplicationModel.Contacts;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Contact picker C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Pick a contact", ClassType=typeof(Scenario1_Single)},
            new Scenario() { Title="Pick multiple contacts", ClassType=typeof(Scenario2_Multiple)},
        };

        static public string GetContactResult(Contact contact)
        {
            var result = new StringBuilder();
            result.AppendFormat("Display name: {0}", contact.DisplayName);
            result.AppendLine();

            foreach (ContactEmail email in contact.Emails)
            {
                result.AppendFormat("Email address ({0}): {1}", email.Kind, email.Address);
                result.AppendLine();
            }

            foreach (ContactPhone phone in contact.Phones)
            {
                result.AppendFormat("Phone ({0}): {1}", phone.Kind, phone.Number);
                result.AppendLine();
            }

            return result.ToString();
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
