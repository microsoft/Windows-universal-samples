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
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml;
using Windows.ApplicationModel.Activation;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "HttpClient sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "GET Text With Cache Control", ClassType = typeof(Scenario01_GetText) },
            new Scenario() { Title = "GET Stream", ClassType = typeof(Scenario02_GetStream) },
            new Scenario() { Title = "GET List", ClassType = typeof(Scenario03_GetList) },
            new Scenario() { Title = "POST Text", ClassType = typeof(Scenario04_PostText) },
            new Scenario() { Title = "POST Stream", ClassType = typeof(Scenario05_PostStream) },
            new Scenario() { Title = "POST Multipart", ClassType = typeof(Scenario06_PostMultipart) },
            new Scenario() { Title = "POST Stream With Progress", ClassType = typeof(Scenario07_PostStreamWithProgress) },
            new Scenario() { Title = "POST Custom Content", ClassType = typeof(Scenario08_PostCustomContent) },
            new Scenario() { Title = "Get Cookies", ClassType = typeof(Scenario09_GetCookie) },
            new Scenario() { Title = "Set Cookie", ClassType = typeof(Scenario10_SetCookie) },
            new Scenario() { Title = "Delete Cookie", ClassType = typeof(Scenario11_DeleteCookie) },
            new Scenario() { Title = "Disable Cookies", ClassType = typeof(Scenario12_DisableCookies) },
            new Scenario() { Title = "Retry Filter", ClassType = typeof(Scenario13_RetryFilter) },
            new Scenario() { Title = "Metered Connection Filter", ClassType = typeof(Scenario14_MeteredConnectionFilter) },
            new Scenario() { Title = "Server Certificate Validation", ClassType = typeof(Scenario15_ServerCertificateValidation) },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

}


