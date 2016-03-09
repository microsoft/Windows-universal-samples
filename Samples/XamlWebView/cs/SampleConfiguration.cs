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
using System.Threading.Tasks;
using Windows.Storage;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "XAML WebView Control";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Navigate to URL", ClassType=typeof(Scenario1_NavToUrl)},
            new Scenario() { Title="Navigate to string", ClassType=typeof(Scenario2_NavToString)},
            new Scenario() { Title="Invoke script", ClassType=typeof(Scenario3_InvokeScript)},
            new Scenario() { Title="Script notify", ClassType=typeof(Scenario4_ScriptNotify)},
            new Scenario() { Title="Navigate to package and local state", ClassType=typeof(Scenario5_NavToFiles)},
            new Scenario() { Title="Navigate with custom resolver", ClassType=typeof(Scenario6_NavToStream)},
            new Scenario() { Title="Share content", ClassType=typeof(Scenario7_Share)},
            new Scenario() { Title="Capture bitmap", ClassType=typeof(Scenario8_CaptureBitmap)},
            new Scenario() { Title="Full-screen mode", ClassType=typeof(Scenario9_ContainsFullScreenElement)},
            new Scenario() { Title="Unviewable content", ClassType=typeof(Scenario10_UnviewableContent) },
        };

        public static async Task<string> LoadStringFromPackageFileAsync(string name)
        {
            // Using the storage classes to read the content from a file as a string.
            StorageFile f = await StorageFile.GetFileFromApplicationUriAsync(new Uri($"ms-appx:///html/{name}"));
            return await FileIO.ReadTextAsync(f);
        }
}

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
