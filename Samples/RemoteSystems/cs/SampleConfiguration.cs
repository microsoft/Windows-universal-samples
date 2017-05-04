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
using System.Collections.ObjectModel;
using Windows.System.RemoteSystems;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "RemoteSystems C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Discovery", ClassType=typeof(Scenario1_Discovery)},
            new Scenario() { Title="Launch URI", ClassType=typeof(Scenario2_LaunchUri)},
            new Scenario() { Title="Launch App Services", ClassType=typeof(Scenario3_LaunchAppServices)},
            new Scenario() { Title="Settings", ClassType=typeof(Scenario4_Settings)},
            new Scenario() { Title="Capabilities", ClassType=typeof(Scenario5_Capabilities)}
        };

        public ObservableCollection<RemoteSystem> systemList = null;
        public Dictionary<string, RemoteSystem> systemMap = null;

        /// <summary>
        /// Checks if the system discovery has been performed and systems have been found.
        /// </summary>
        internal bool ValidateSystemDiscovery()
        {
            if (systemList == null)
            {
                NotifyUser("System discovery has not been performed. Use scenario one to discover systems.", NotifyType.ErrorMessage);
                return false;
            }
            else if (systemList.Count == 0)
            {
                NotifyUser("No systems have been found. Use scenario one to discover systems.", NotifyType.ErrorMessage);
                return false;
            }

            return true;
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
