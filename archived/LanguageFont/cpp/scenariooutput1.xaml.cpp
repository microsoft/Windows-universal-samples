// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

//
// Scenario1Output.xaml.cpp
// Implementation of the Scenario1Output class
//

#include "pch.h"
#include "ScenarioOutput1.xaml.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

ScenarioOutput1::ScenarioOutput1()
{
    InitializeComponent();
}

ScenarioOutput1::~ScenarioOutput1()
{
}

#pragma region Template-Related Code - Do not remove
void ScenarioOutput1::OnNavigatedTo(NavigationEventArgs^ e)
{
    // Get a pointer to our main page.
    rootPage = dynamic_cast<MainPage^>(e->Parameter);

    // We want to be notified with the OutputFrame is loaded so we can get to the content.
    _frameLoadedToken = rootPage->InputFrameLoaded += ref new Windows::Foundation::EventHandler<Platform::Object^>(this, &ScenarioOutput1::rootPage_InputFrameLoaded);
}

void ScenarioOutput1::OnNavigatedFrom(NavigationEventArgs^ e)
{
    rootPage->InputFrameLoaded -= _frameLoadedToken;
}

#pragma endregion

#pragma region Use this code if you need access to elements in the input frame - otherwise delete
void ScenarioOutput1::rootPage_InputFrameLoaded(Object^ sender, Object^ e)
{
    // Get a pointer to the content within the OutputFrame.
    Page^ inputFrame = dynamic_cast<Page^>(rootPage->InputFrame->Content);

    // Go find the elements that we need for this scenario
    // ex: flipView1 = dynamic_cast<FlipView^>(inputFrame->FindName("FlipView1"));
}

#pragma endregion
