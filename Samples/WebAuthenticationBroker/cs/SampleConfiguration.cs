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
using WebAuthentication;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Web authentication broker sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Connect to Facebook Services", ClassType = typeof(Scenario1_Facebook) },
            new Scenario() { Title = "Connect to Twitter Services", ClassType = typeof(Scenario2_Twitter) },
            new Scenario() { Title = "Connect to Flickr Services", ClassType = typeof(Scenario3_Flickr) },
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    public static class FormDecoderExtensions
    {
        public static string TryGetValue(this Windows.Foundation.WwwFormUrlDecoder decoder, string name)
        {
            try
            {
                return decoder.GetFirstValueByName(name);
            }
            catch (ArgumentException)
            {
                return null;
            }
        }
    }
}

