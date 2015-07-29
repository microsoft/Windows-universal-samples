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
#include "Scenario1_Basic.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;

Scenario1_Basic::Scenario1_Basic() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario1_Basic::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    // The SizeChanged event is raised when the user interaction mode changes.
    windowResizeToken = Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &Scenario1_Basic::OnWindowResize);
    UpdateContent();
}

void Scenario1_Basic::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    Window::Current->SizeChanged -= windowResizeToken;
}

void Scenario1_Basic::OnWindowResize(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e)
{
    UpdateContent();
}

void Scenario1_Basic::UpdateContent()
{
    InteractionMode = UIViewSettings::GetForCurrentView()->UserInteractionMode;

    // Update styles
    CheckBoxStyle = InteractionMode == UserInteractionMode::Mouse ? MouseCheckBoxStyle : TouchCheckBoxStyle;
}
