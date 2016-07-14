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
using Windows.ApplicationModel.Background;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "CallerID";

        List<Scenario> scenarios = new List<Scenario>
        {            
            new Scenario() { Title="Register the background task", ClassType=typeof(Scenario1_Register)},
            new Scenario() { Title="Call Origin Setting", ClassType=typeof(Scenario2_SetDefault)},
        };

        public const string BackgroundTaskEntryPoint = "CallerIDBackgroundTask.SampleCallerIDBackgroundTask";
        public const string BackgroundTaskName = "SampleCallerIDBackgroundTask";

        public static IBackgroundTaskRegistration FindBackgroundTaskRegistration()
        {
            foreach (var taskRegistration in BackgroundTaskRegistration.AllTasks.Values)
            {
                if (taskRegistration.Name == BackgroundTaskName)
                {
                    return taskRegistration;
                }
            }
            return null;
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
