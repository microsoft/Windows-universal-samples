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
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "SampleConfiguration.h"
#include "ViewLifetimeControl.h"

using namespace winrt;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"Multiple Views";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Creating and showing multiple views", xaml_typename<SDKTemplate::Scenario1>() },
    Scenario{ L"Using animations when switching", xaml_typename<SDKTemplate::Scenario3>() },
});

implementation::App* SampleState::CurrentApp = nullptr;
IObservableVector<IInspectable> SampleState::SecondaryViews = single_threaded_observable_vector<IInspectable>();
CoreDispatcher SampleState::MainDispatcher{ nullptr };
int32_t SampleState::MainViewId = 0;

namespace
{
    ViewLifetimeControl TryFindViewLifetimeControlForViewId(int32_t viewId)
    {
        for (IInspectable const& item : SampleState::SecondaryViews)
        {
            auto viewLifetimeControl = item.as<ViewLifetimeControl>();
            if (viewLifetimeControl.Id() == viewId)
            {
                return viewLifetimeControl;
            }
        }
        return nullptr;
    }
}

// This method is designed to always be run on the thread that's
// binding to the list above
void SampleState::UpdateTitle(hstring newTitle, int32_t viewId)
{
    ViewLifetimeControl foundData = TryFindViewLifetimeControlForViewId(viewId);
    if (foundData)
    {
        foundData.Title(newTitle);
    }
    else
    {
        throw hresult_error(HRESULT_FROM_WIN32(ERROR_NO_MATCH), L"Couldn't find the view ID in the collection");
    }
}

bool winrt::SDKTemplate::App_OverrideOnLaunched(LaunchActivatedEventArgs const& e)
{
    // Check if a secondary view is supposed to be shown
    ViewLifetimeControl viewLifetimeControl = TryFindViewLifetimeControlForViewId(e.CurrentlyShownApplicationViewId());
    if (viewLifetimeControl)
    {
        viewLifetimeControl.Dispatcher().RunAsync(CoreDispatcherPriority::Normal, [=]()
        {
            Window::Current().Activate();
        });
        return true;
    }

    // Activating the main view.
    if (!SampleState::MainDispatcher)
    {
        SampleState::MainDispatcher = Window::Current().Dispatcher();
        SampleState::MainViewId = ApplicationView::GetForCurrentView().Id();
    }
    return false;
}

fire_and_forget winrt::SDKTemplate::App_OnActivated(IActivatedEventArgs const& e)
{
    if (e.Kind() == ActivationKind::Protocol)
    {
        auto protocolArgs = e.as<ProtocolActivatedEventArgs>();

        // Capture the activation arguments before we co_await.
        Uri uri = protocolArgs.Uri();

        // Find out which window the system chose to display
        // Unless you've set DisableShowingMainViewOnActivation,
        // it will always be your main view.
        ViewLifetimeControl viewControl = TryFindViewLifetimeControlForViewId(protocolArgs.CurrentlyShownApplicationViewId());
        if (viewControl)
        {
            co_await resume_foreground(viewControl.Dispatcher());

            // We are now in the secondary page.
            auto currentPage = Window::Current().Content().as<Frame>().Content().as<SecondaryViewPage>();
            currentPage.HandleProtocolLaunch(uri);
            Window::Current().Activate();
        }
        else
        {
            // We don't have the specified view in the collection, likely because it's the main view
            // that got shown. Set up the main view to display.
            Frame rootFrame = SampleState::CurrentApp->CreateRootFrame();
            if (rootFrame.Content() == nullptr)
            {
                rootFrame.Navigate(xaml_typename<SDKTemplate::MainPage>());
                SampleState::MainDispatcher = Window::Current().Dispatcher();
                SampleState::MainViewId = ApplicationView::GetForCurrentView().Id();
            }

            auto rootPage = MainPage::Current();
            rootPage.Navigate(xaml_typename<Scenario3>(), nullptr);
                rootPage.NotifyUser(L"Main window was launched with protocol: " + uri.AbsoluteUri(),
                    NotifyType::StatusMessage);
            Window::Current().Activate();
        }
    }
}
