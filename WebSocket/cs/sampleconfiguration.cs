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

using System.Collections.Generic;
using Windows.UI.Xaml.Controls;using System;
using Microsoft.Samples.Networking.WebSocket;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        // Change the string below to reflect the name of your sample.
        // This is used on the main page as the title of the sample.
        public const string FEATURE_NAME = "WebSocket";

        // Change the array below to reflect the name of your scenarios.
        // This will be used to populate the list of scenarios on the main page with
        // which the user will choose the specific scenario that they are interested in.
        // These should be in the form: "Navigating to a web page".
        // The code in MainPage will take care of turning this into: "1) Navigating to a web page"
        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "UTF-8 text messages", ClassType = typeof(Scenario1) },
            new Scenario() { Title = "Binary data stream", ClassType = typeof(Scenario2) },
        };

        public bool TryGetUri(string uriString, out Uri uri)
        {
            uri = null;

            Uri webSocketUri;
            if (!Uri.TryCreate(uriString.Trim(), UriKind.Absolute, out webSocketUri))
            {
                NotifyUser("Error: Invalid URI", NotifyType.ErrorMessage);
                return false;
            }
            
            // Fragments are not allowed in WebSocket URIs.
            if (!String.IsNullOrEmpty(webSocketUri.Fragment))
            {
                NotifyUser("Error: URI fragments not supported in WebSocket URIs.", NotifyType.ErrorMessage);
                return false;
            }

            // Uri.SchemeName returns the canonicalized scheme name so we can use case-sensitive, ordinal string
            // comparison.
            if ((webSocketUri.Scheme != "ws") && (webSocketUri.Scheme != "wss"))
            {
                NotifyUser("Error: WebSockets only support ws:// and wss:// schemes.", NotifyType.ErrorMessage);
                return false;
            }

            uri = webSocketUri;

            return true;
        }
    }

    public class Scenario
    {
        public string Title { get; set; }

        public Type ClassType { get; set; }

        public override string ToString()
        {
            return Title;
        }
    }
}
