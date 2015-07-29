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
#include "ProjectionViewPage.xaml.h"
#include "MainPage.xaml.h"
#include "ViewLifetimeControl.h"
#include "SampleConfiguration.h"

using namespace SDKSample;
using namespace SDKSample::SecondaryViewsHelpers;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

ProjectionViewPage::ProjectionViewPage()
{
    InitializeComponent();
}

void ProjectionViewPage::OnNavigatedTo(NavigationEventArgs^ e)
{
    auto initData = dynamic_cast<ProjectionViewPageInitializationData^>(e->Parameter);

    // The ViewLifetimeControl is a convenient wrapper that ensures the
    // view is closed only when the user is done with it
    thisViewControl = initData->ProjectionViewPageControl;
    mainDispatcher = initData->MainDispatcher;
    mainViewId = initData->MainViewId;

    // Listen for when it's time to close this view
    releasedToken = thisViewControl->Released += ref new ViewReleasedHandler(this, &ProjectionViewPage::thisViewControl_Released);
}

void ProjectionViewPage::thisViewControl_Released(Object^ sender, Object^ e)
{
    // There are two major cases where this event will get invoked:
    // 1. The view goes unused for some time, and the system cleans it up
    // 2. The app calls "StopProjectingAsync"
    thisViewControl->Released -= releasedToken;
    auto thisDispatcher = Window::Current->Dispatcher;

    create_task(mainDispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([thisDispatcher]()
    {
        MainPage::Current->ProjectionViewPageControl = nullptr;
        thisDispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([]() {
            Window::Current->Close();
        }));
    })));

}

void ProjectionViewPage::SwapViews_Click(Object^ sender, RoutedEventArgs^ e)
{
    // The view might arrive on the wrong display. The user can
    // easily swap the display on which the view appears
    thisViewControl->StartViewInUse();
    create_task(ProjectionManager::SwapDisplaysForViewsAsync(
        ApplicationView::GetForCurrentView()->Id,
        mainViewId
        )).then([this]()
    {
        thisViewControl->StopViewInUse();
    });
}

void ProjectionViewPage::StopProjecting_Click(Object^ sender, RoutedEventArgs^ e)
{
    // There may be cases to end the projection from the projected view
    // (e.g. the presentation hosted in that view concludes)
    thisViewControl->StartViewInUse();
    create_task(ProjectionManager::StopProjectingAsync(
        ApplicationView::GetForCurrentView()->Id,
        mainViewId
        )).then([this]()
    {
        thisViewControl->StopViewInUse();
    });
}
