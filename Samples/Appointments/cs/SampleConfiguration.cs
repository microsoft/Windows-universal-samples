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
using Windows.Foundation;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Appointments";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Define an Appointment", ClassType = typeof(Scenario1_Define) },
            new Scenario() { Title = "Add an Appointment", ClassType = typeof(Scenario2_Add) },
            new Scenario() { Title = "Replace an Appointment", ClassType = typeof(Scenario3_Replace) },
            new Scenario() { Title = "Remove an Appointment", ClassType = typeof(Scenario4_Remove) },
            new Scenario() { Title = "Show Time Frame", ClassType = typeof(Scenario5_Show) },
            new Scenario() { Title = "Recurring Appointments", ClassType = typeof(Scenario6_Recurrence) },
        };

        // Helper function to calculate an element's rectangle in root-relative coordinates.
        public static Rect GetElementRect(Windows.UI.Xaml.FrameworkElement element)
        {
            Windows.UI.Xaml.Media.GeneralTransform transform = element.TransformToVisual(null);
            Point point = transform.TransformPoint(new Point());
            return new Rect(point, new Size(element.ActualWidth, element.ActualHeight));
        }

    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
