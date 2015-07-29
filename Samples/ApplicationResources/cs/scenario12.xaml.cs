//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using Windows.ApplicationModel.Resources.Core;
using System.Collections.Generic;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario12 : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario12()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }

        private async void Scenario12Button_Show_Click(object sender, RoutedEventArgs e)
        {
            // Two coding patterns will be used:
            //   1. Get a ResourceContext on the UI thread using GetForCurrentView and pass 
            //      to the non-UI thread
            //   2. Get a ResourceContext on the non-UI thread using GetForViewIndependentUse
            //
            // Two analogous patterns could be used for ResourceLoader instead of ResourceContext.


            // pattern 1: get a ResourceContext for the UI thread
            ResourceContext defaultContextForUiThread = ResourceContext.GetForCurrentView();

            // pattern 2: we'll create a view-independent context in the non-UI worker thread


            // We need some things in order to display results in the UI (doing that
            // for purposes of this sample, to show that work was actually done in the
            // worker thread):
            List<string> uiDependentResourceList = new List<string>();
            List<string> uiIndependentResourceList = new List<string>();


            // use a worker thread for the heavy lifting so the UI isn't blocked
            await Windows.System.Threading.ThreadPool.RunAsync(
                (source) =>
                {
                    ResourceMap stringResourceMap = ResourceManager.Current.MainResourceMap.GetSubtree("Resources");

                    // pattern 1: the defaultContextForUiThread variable was created above and is visible here

                    // pattern 2: get a view-independent ResourceContext
                    ResourceContext defaultViewIndependentResourceContext = ResourceContext.GetForViewIndependentUse();

                    // NOTE: The ResourceContext obtained using GetForViewIndependentUse() has no scale qualifier
                    // value set. If this ResourceContext is used in its default state to retrieve a resource, that 
                    // will work provided that the resource does not have any scale-qualified variants. But if the
                    // resource has any scale-qualified variants, then that will fail at runtime.
                    //
                    // A scale qualifier value on this ResourceContext can be set programmatically. If that is done,
                    // then the ResourceContext can be used to retrieve a resource that has scale-qualified variants.
                    // But if the scale qualifier is reset (e.g., using the ResourceContext.Reset() method), then
                    // it will return to the default state with no scale qualifier value set, and cannot be used
                    // to retrieve any resource that has scale-qualified variants.


                    // simulate processing a number of items
                    // just using a single string resource: that's sufficient to demonstrate 
                    for (var i = 0; i < 4; i++)
                    {
                        // pattern 1: use the ResourceContext from the UI thread
                        string listItem1 = stringResourceMap.GetValue("string1", defaultContextForUiThread).ValueAsString;
                        uiDependentResourceList.Add(listItem1);

                        // pattern 2: use the view-independent ResourceContext
                        string listItem2 = stringResourceMap.GetValue("string1", defaultViewIndependentResourceContext).ValueAsString;
                        uiIndependentResourceList.Add(listItem2);
                    }

                });

            // Display the results in one go. (A more finessed design might add results
            // in the UI asynchronously, but that goes beyond what this sample is 
            // demonstrating.)
            ViewDependentResourcesList.ItemsSource = uiDependentResourceList;
            ViewIndependentResourcesList.ItemsSource = uiIndependentResourceList;
        }

    }
}
