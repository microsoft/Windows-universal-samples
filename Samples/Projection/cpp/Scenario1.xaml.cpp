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

#include "pch.h"
#include "Scenario1.xaml.h"
#include "MainPage.xaml.h"
#include "ProjectionViewPage.xaml.h"
#include "ViewLifetimeControl.h"

using namespace SDKSample;

using namespace Concurrency;
using namespace Platform;
using namespace SDKSample::SecondaryViewsHelpers;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;

Scenario1::Scenario1()
{
    InitializeComponent();
    thisViewId = ApplicationView::GetForCurrentView()->Id;
}

void Scenario1::StartProjecting_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto prerequisite = task<void>([]() {});
    // If projection is already in progress, then it could be shown on the monitor again
    // Otherwise, we need to create a new view to show the presentation
    if (MainPage::Current->ProjectionViewPageControl == nullptr)
    {
        // First, create a new, blank view
        auto mainDispatcher = Window::Current->Dispatcher;
        int mainViewId = thisViewId;
        prerequisite = create_task(CoreApplication::CreateNewView()->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([mainViewId, mainDispatcher]()
        {
            // ViewLifetimeControl is a wrapper to make sure the view is closed only
            // when the app is done with it
            MainPage::Current->ProjectionViewPageControl = ViewLifetimeControl::CreateForCurrentView();

            // Assemble some data necessary for the new page
            auto initData = ref new ProjectionViewPageInitializationData();
            initData->MainDispatcher = mainDispatcher;
            initData->ProjectionViewPageControl = MainPage::Current->ProjectionViewPageControl;
            initData->MainViewId = mainViewId;

            // Display the page in the view. Note that the view will not become visible
            // until "StartProjectingAsync" is called
            auto rootFrame = ref new Windows::UI::Xaml::Controls::Frame();
            rootFrame->Navigate(TypeName(ProjectionViewPage::typeid), initData);
            Window::Current->Content = rootFrame;

            // The call to Window.Current.Activate is required starting in Windos 10.
            // Without it, the view will never appear.
            Window::Current->Activate();
        })));
    }

    prerequisite.then([this]()
    {
        try
        {
            // Start/StopViewInUse are used to signal that the app is interacting with the
            // view, so it shouldn't be closed yet, even if the user loses access to it
            MainPage::Current->ProjectionViewPageControl->StartViewInUse();

            // Show the view on a second display (if available) or on the primary display
            create_task(ProjectionManager::StartProjectingAsync(
                MainPage::Current->ProjectionViewPageControl->Id,
                thisViewId
                )).then([]() {
                MainPage::Current->ProjectionViewPageControl->StopViewInUse();
            });
        }
        catch (ObjectDisposedException^)
        {
            // The projection view is being disposed
            MainPage::Current->NotifyUser("The projection view is being disposed", NotifyType::ErrorMessage);   
        }
    }, task_continuation_context::use_arbitrary());
}
