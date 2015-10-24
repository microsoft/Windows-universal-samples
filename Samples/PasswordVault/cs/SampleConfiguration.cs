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
using Windows.Security.Credentials;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Password vault C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Save credentials", ClassType=typeof(Scenario1_Save) },
            new Scenario() { Title="Manage credentials", ClassType=typeof(Scenario2_Manage) },
        };

        // Value is not important. This is just a trick to run code at construction.
        private bool initialize = InitializePasswordVaultInTheBackground();

        static bool InitializePasswordVaultInTheBackground()
        {
            // Explicitly place this task on a background thread.
            Task.Factory.StartNew(() =>
            {
                // any call to the password vault will load the vault.
                var vault = new PasswordVault();
                vault.RetrieveAll();
            });
            return true;
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
