//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System.Collections.Generic;
using System;
using SDKTemplate;

namespace SDKTemplate
{
    public partial class MainPage
    {
        // Change the string below to reflect the name of your sample.
        // This is used on the main page as the title of the sample.
        public const string FEATURE_NAME = "Application Resources C# sample";

        // Change the array below to reflect the name of your scenarios.
        // This will be used to populate the list of scenarios on the main page with
        // which the user will choose the specific scenario that they are interested in.
        // These should be in the form: "Navigating to a web page".
        // The code in MainPage will take care of turning this into: "1) Navigating to a web page"
        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "String Resources In XAML", ClassType = typeof(Scenario1) },
            new Scenario() { Title = "File Resources In XAML", ClassType = typeof(Scenario2) },
            new Scenario() { Title = "String Resources In Code", ClassType = typeof(Scenario3) },
            new Scenario() { Title = "Resources in the AppX manifest", ClassType = typeof(Scenario4) },
            new Scenario() { Title = "Additional Resource Files", ClassType = typeof(Scenario5) },
            new Scenario() { Title = "Class Library Resources", ClassType = typeof(Scenario6) },
            new Scenario() { Title = "Runtime Changes/Events", ClassType = typeof(Scenario7) },
            new Scenario() { Title = "Application Languages", ClassType = typeof(Scenario8) },
            new Scenario() { Title = "Override Languages", ClassType = typeof(Scenario9) },
            new Scenario() { Title = "Multi-dimensional fallback", ClassType = typeof(Scenario10) },
            new Scenario() { Title = "Working with webservices", ClassType = typeof(Scenario11) },
            new Scenario() { Title = "Retrieving resources in non-UI threads", ClassType = typeof(Scenario12) },
            new Scenario() { Title = "File resources in code", ClassType = typeof(Scenario13) },
        };
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
