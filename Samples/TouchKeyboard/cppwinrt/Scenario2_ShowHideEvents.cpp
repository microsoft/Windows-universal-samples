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
#include "Scenario2_ShowHideEvents.h"
#include "Scenario2_ShowHideEvents.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::ViewManagement;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_ShowHideEvents::Scenario2_ShowHideEvents()
    {
        InitializeComponent();
    }

    void Scenario2_ShowHideEvents::OnNavigatedTo(NavigationEventArgs const&)
    {
        InputPane currentInputPane = InputPane::GetForCurrentView();

        // Subscribe to Showing/Hiding events
        showingEventToken = currentInputPane.Showing({ get_weak(), &Scenario2_ShowHideEvents::OnShowing });
        hidingEventToken = currentInputPane.Hiding({ get_weak(), &Scenario2_ShowHideEvents::OnHiding });
    }

    void Scenario2_ShowHideEvents::OnNavigatedFrom(NavigationEventArgs const&)
    {
        InputPane currentInputPane = InputPane::GetForCurrentView();

        // Unsubscribe from Showing/Hiding events
        currentInputPane.Showing(showingEventToken);
        currentInputPane.Hiding(hidingEventToken);
    }

    void Scenario2_ShowHideEvents::OnShowing(InputPane const&, InputPaneVisibilityEventArgs const&)
    {
        LastInputPaneEventRun().Text(L"Showing");
    }

    void Scenario2_ShowHideEvents::OnHiding(InputPane const&, InputPaneVisibilityEventArgs const&)
    {
        LastInputPaneEventRun().Text(L"Hiding");
    }
}
