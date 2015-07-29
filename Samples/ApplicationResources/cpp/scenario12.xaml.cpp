//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario12.xaml.cpp
// Implementation of the Scenario12 class
//

#include <ppltasks.h>
#include "pch.h"
#include "Scenario12.xaml.h"

using namespace SDKTemplate;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::ApplicationModel::Resources::Core;
using namespace concurrency;

Scenario12::Scenario12()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario12::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}


void Scenario12::Scenario12Button_Show_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Two coding patterns will be used:
    //   1. Get a ResourceContext on the UI thread using GetForCurrentView and pass 
    //      to the non-UI thread
    //   2. Get a ResourceContext on the non-UI thread using GetForViewIndependentUse
    //
    // Two analogous patterns could be used for ResourceLoader instead of ResourceContext.

    // pattern 1: get a ResourceContext for the UI thread
    ResourceContext^ defaultContextForUiThread = ResourceContext::GetForCurrentView();

    // pattern 2: we'll create a view-independent context in the non-UI worker thread


    // We need some things in order to display results in the UI (doing that
    // for purposes of this sample, to show that work was actually done in the
    // worker thread): a pair of vectors to capture data, and a pair of variable 
    // references to the controls where the results will be displayed (needed to
    // pass to the .then lambda).
    Platform::Collections::Vector<Platform::String^>^ uiDependentResourceItems = ref new Platform::Collections::Vector<Platform::String^>();
    Platform::Collections::Vector<Platform::String^>^ uiIndependentResourceItems = ref new Platform::Collections::Vector<Platform::String^>();
    ItemsControl^ viewDependentListControl = ViewDependentResourcesList;
    ItemsControl^ viewIndependentListControl = ViewIndependentResourcesList;


    // use a worker thread for the heavy lifting so the UI isn't blocked
    concurrency::create_task(
        Windows::System::Threading::ThreadPool::RunAsync(
            ref new Windows::System::Threading::WorkItemHandler(
            [defaultContextForUiThread, uiDependentResourceItems, uiIndependentResourceItems](Windows::Foundation::IAsyncAction^ /*action*/)
        {
            // This is happening asynchronously on a background worker thread,
            // not on the UI thread.

            ResourceMap^ stringResourceMap = ResourceManager::Current->MainResourceMap->GetSubtree("Resources");


            // coding pattern 1: the defaultContextForUiThread variable was created above and has been captured to use here

            // coding pattern 2: get a view-independent ResourceContext
            ResourceContext^ defaultViewIndependentResourceContext = ResourceContext::GetForViewIndependentUse();

            // NOTE: The ResourceContext obtained using GetForViewIndependentUse() has no scale qualifier
            // value set. If this ResourceContext is used in its default state to retrieve a resource, that 
            // will work provided that the resource does not have any scale-qualified variants. But if the
            // resource has any scale-qualified variants, then that will fail at runtime.
            //
            // A scale qualifier value on this ResourceContext can be set programmatically. If that is done,
            // then the ResourceContext can be used to retrieve a resource that has scale-qualified variants.
            // But if the scale qualifier is reset (e.g., using the ResourceContext::Reset() method), then
            // it will return to the default state with no scale qualifier value set, and cannot be used
            // to retrieve any resource that has scale-qualified variants.


            // simulate processing a number of items
            // just using a single string resource: that's sufficient to demonstrate 
            for (auto i = 0; i < 4; i++)
            {
                // pattern 1: use the ResourceContext from the UI thread
                Platform::String^ item1 = stringResourceMap->GetValue("string1", defaultContextForUiThread)->ValueAsString;
                uiDependentResourceItems->Append(item1);

                // pattern 2: use the view-independent ResourceContext
                Platform::String^ item2 = stringResourceMap->GetValue("string1", defaultViewIndependentResourceContext)->ValueAsString;
                uiIndependentResourceItems->Append(item2);
            }

        }))

    ).then([uiDependentResourceItems, uiIndependentResourceItems, viewDependentListControl, viewIndependentListControl]
    {
        // After the async work is complete, this will happen on the UI thread.

        // Display the results in one go. (A more finessed design might add results
        // in the UI asynchronously, but that goes beyond what this sample is 
        // demonstrating.)
        viewDependentListControl->ItemsSource = uiDependentResourceItems;
        viewIndependentListControl->ItemsSource = uiIndependentResourceItems;
    });

}