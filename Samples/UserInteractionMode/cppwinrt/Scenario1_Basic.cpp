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
#include "Scenario1_Basic.h"
#include "Scenario1_Basic.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Basic::Scenario1_Basic()
    {
        InitializeComponent();
    }

    void Scenario1_Basic::OnNavigatedTo(NavigationEventArgs const&)
    {
        rootPage = MainPage::Current();

        // The SizeChanged event is raised when the user interaction mode changes.
        windowResizeToken = Window::Current().SizeChanged({ get_weak(), &Scenario1_Basic::OnWindowResize });
        UpdateContent();
    }

    void Scenario1_Basic::OnNavigatedFrom(NavigationEventArgs const&)
    {
        Window::Current().SizeChanged(windowResizeToken);
    }

    void Scenario1_Basic::OnWindowResize(IInspectable const&, WindowSizeChangedEventArgs const&)
    {
        UpdateContent();
    }

    void Scenario1_Basic::UpdateContent()
    {
        m_interactionMode = UIViewSettings::GetForCurrentView().UserInteractionMode();

        // Update styles
        m_checkBoxStyle = InteractionMode() == UserInteractionMode::Mouse ? MouseCheckBoxStyle() : TouchCheckBoxStyle();

        m_propertyChanged(*this, PropertyChangedEventArgs(L"InteractionMode"));
        m_propertyChanged(*this, PropertyChangedEventArgs(L"CheckBoxStyle"));
    }

    event_token Scenario1_Basic::PropertyChanged(PropertyChangedEventHandler const& handler)
    {
        return m_propertyChanged.add(handler);
    }

    void Scenario1_Basic::PropertyChanged(event_token const& token) noexcept
    {
        m_propertyChanged.remove(token);
    }
}
