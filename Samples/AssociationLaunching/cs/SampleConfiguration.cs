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
using Windows.ApplicationModel.Activation;
using Windows.Foundation;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Association Launching C# Sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Launching a file", ClassType = typeof(Scenario1_LaunchFile) },
            new Scenario() { Title = "Launching a URI", ClassType = typeof(Scenario2_LaunchUri) },
            new Scenario() { Title = "Receiving a file", ClassType = typeof(Scenario3_ReceiveFile) },
            new Scenario() { Title = "Receiving a URI", ClassType = typeof(Scenario4_ReceiveUri) }
        };

        static public List<ViewSizePreference> ViewSizePreferences = new List<ViewSizePreference>
        {
            ViewSizePreference.Default,
            ViewSizePreference.UseLess,
            ViewSizePreference.UseHalf,
            ViewSizePreference.UseMore,
            ViewSizePreference.UseMinimum,
            ViewSizePreference.UseNone,
        };

        public void NavigateToPageWithParameter(int pageIndex, object parameter)
        {
            ScenarioControl.SelectedIndex = pageIndex;
            ScenarioFrame.Navigate(scenarios[pageIndex].ClassType, parameter);
        }

        static public Point GetElementLocation(object e)
        {
            var element = (FrameworkElement)e;
            Windows.UI.Xaml.Media.GeneralTransform buttonTransform = element.TransformToVisual(null);

            Point desiredLocation = buttonTransform.TransformPoint(new Point());
            desiredLocation.Y = desiredLocation.Y + element.ActualHeight;

            return desiredLocation;
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

    public partial class App : Application
    {
        /// <summary>
        // Handle file activations.
        /// </summary>
        protected override void OnFileActivated(FileActivatedEventArgs e)
        {
            Frame rootFrame = CreateRootFrame();

            if (rootFrame.Content == null)
            {
                if (!rootFrame.Navigate(typeof(MainPage)))
                {
                    throw new Exception("Failed to create initial page");
                }
            }

            var p = rootFrame.Content as MainPage;
            p.NavigateToPageWithParameter(2, e);

            // Ensure the current window is active
            Window.Current.Activate();
        }

        /// <summary>
        // Handle protocol activations.
        /// </summary>
        protected override void OnActivated(IActivatedEventArgs e)
        {
            if (e.Kind == ActivationKind.Protocol)
            {
                Frame rootFrame = CreateRootFrame();

                if (rootFrame.Content == null)
                {
                    if (!rootFrame.Navigate(typeof(MainPage)))
                    {
                        throw new Exception("Failed to create initial page");
                    }
                }

                var p = rootFrame.Content as MainPage;
                p.NavigateToPageWithParameter(3, e);

                // Ensure the current window is active
                Window.Current.Activate();
            }
        }

    }
}
