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
        public const string FEATURE_NAME = "Splash screen C# sample";
        internal bool Dismissed;
        internal Rect SplashImageRect;

        public List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Registering for dismissal notifications", ClassType = typeof(SplashScreenSample.Scenario1) },
            new Scenario() { Title = "Extending the splash screen",             ClassType = typeof(SplashScreenSample.Scenario2) },
        };

        public void SetExtendedSplashInfo(Rect splashRect, bool dismissStat)
        {
            SplashImageRect = splashRect;
            Dismissed = dismissStat;
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

    public class MainPageSizeChangedEventArgs : EventArgs
    {
        private double width;

        public double Width
        {
            get { return width; }
            set { width = value; }
        }
    }

    public enum NotifyType
    {
        StatusMessage,
        ErrorMessage
    };
}