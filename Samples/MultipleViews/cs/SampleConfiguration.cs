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

using SecondaryViewsHelpers;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Windows.ApplicationModel.Activation;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage
    {
        public const string FEATURE_NAME = "Multiple Views";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Creating and showing multiple views", ClassType = typeof(Scenario1) },
            new Scenario() { Title = "Using animations when switching",     ClassType = typeof(Scenario3) }
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

    public partial class App
    {
        partial void OverrideOnLaunched(LaunchActivatedEventArgs args, ref bool handled)
        {
            // Check if a secondary view is supposed to be shown
            ViewLifetimeControl ViewLifetimeControl;
            handled = TryFindViewLifetimeControlForViewId(args.CurrentlyShownApplicationViewId, out ViewLifetimeControl);
            if (handled)
            {
                var task = ViewLifetimeControl.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    Window.Current.Activate();
                });
            }
        }

        partial void InitializeRootFrame(Frame frame)
        {
            mainDispatcher = Window.Current.Dispatcher;
            mainViewId = ApplicationView.GetForCurrentView().Id;
        }

        protected async override void OnActivated(IActivatedEventArgs args)
        {
            if (args.Kind == ActivationKind.Protocol)
            {
                var protocolArgs = (ProtocolActivatedEventArgs)args;
                // Find out which window the system chose to display
                // Unless you've set DisableShowingMainViewOnActivation,
                // it will always be your most recent view.
                ViewLifetimeControl ViewLifetimeControl;
                if (TryFindViewLifetimeControlForViewId(protocolArgs.CurrentlyShownApplicationViewId, out ViewLifetimeControl))
                {
                    await ViewLifetimeControl.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        var currentPage = (SecondaryViewPage)((Frame)Window.Current.Content).Content;
                        currentPage.HandleProtocolLaunch(protocolArgs.Uri);
                        Window.Current.Activate();
                    });
                }
                else
                {
                    // We don't have the specified view in the collection, likely because it's the main view
                    // that got shown. Set up the main view to display.
                    InitializeMainPage(args.PreviousExecutionState, "");
                    var rootPage = (MainPage)((Frame)Window.Current.Content).Content;
                    ((ListBox)rootPage.FindName("Scenarios")).SelectedIndex = 1;
                    rootPage.NotifyUser("Main window was launched with protocol: " + protocolArgs.Uri.AbsoluteUri,
                                        NotifyType.StatusMessage);
                    Window.Current.Activate();
                }
            }
        }

        public ObservableCollection<ViewLifetimeControl> SecondaryViews = new ObservableCollection<ViewLifetimeControl>();

        // This method is designed to always be run on the thread that's
        // binding to the list above
        public void UpdateTitle(String newTitle, int viewId)
        {
            ViewLifetimeControl foundData;
            if (TryFindViewLifetimeControlForViewId(viewId, out foundData))
            {
                foundData.Title = newTitle;
            }
            else
            {
                throw new KeyNotFoundException("Couldn't find the view ID in the collection");
            }
        }

        // Loop through the collection to find the view ID
        // This should only be run on the main thread.
        bool TryFindViewLifetimeControlForViewId(int viewId, out ViewLifetimeControl foundData)
        {
            foreach (var ViewLifetimeControl in SecondaryViews)
            {
                if (ViewLifetimeControl.Id == viewId)
                {
                    foundData = ViewLifetimeControl;
                    return true;
                }
            }
            foundData = null;
            return false;
        }

        private CoreDispatcher mainDispatcher;
        public CoreDispatcher MainDispatcher
        {
            get
            {
                return mainDispatcher;
            }
        }

        private int mainViewId;
        public int MainViewId
        {
            get
            {
                return mainViewId;
            }
        }

    }
}
