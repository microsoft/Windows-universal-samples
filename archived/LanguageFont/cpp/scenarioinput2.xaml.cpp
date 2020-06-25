// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

//
// Scenario2Input.xaml.cpp
// Implementation of the Scenario2Input class.
//

#include "pch.h"
#include "ScenarioInput2.xaml.h"
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
using namespace Windows::Globalization::Fonts;

ScenarioInput2::ScenarioInput2()
{
    InitializeComponent();
}

ScenarioInput2::~ScenarioInput2()
{
}

#pragma region Template-Related Code - Do not remove
void ScenarioInput2::OnNavigatedTo(NavigationEventArgs^ e)
{
    // Get a pointer to our main page.
    rootPage = dynamic_cast<MainPage^>(e->Parameter);

    // We want to be notified with the OutputFrame is loaded so we can get to the content.
    _frameLoadedToken = rootPage->OutputFrameLoaded += ref new Windows::Foundation::EventHandler<Platform::Object^>(this, &ScenarioInput2::rootPage_OutputFrameLoaded);
}

void ScenarioInput2::OnNavigatedFrom(NavigationEventArgs^ e)
{
    rootPage->OutputFrameLoaded -= _frameLoadedToken;
    if (_oriHeadingUI != nullptr)
    {
        Page^ outputFrame = dynamic_cast<Page^>(rootPage->OutputFrame->Content);
        auto headingUI = dynamic_cast<TextBlock^>(outputFrame->FindName("Scenario2Heading"));
        auto textUI = dynamic_cast<TextBlock^>(outputFrame->FindName("Scenario2Text"));
        _oriHeadingUI->Reset(headingUI);
        _oriTextUI->Reset(textUI);
    }
}

#pragma endregion

#pragma region Use this code if you need access to elements in the output frame - otherwise delete
void ScenarioInput2::rootPage_OutputFrameLoaded(Object^ sender, Object^ e)
{
    // Get a pointer to the content within the OutputFrame.
    Page^ outputFrame = dynamic_cast<Page^>(rootPage->OutputFrame->Content);

    // Go find the elements that we need for this scenario
    // ex: flipView1 = dynamic_cast<FlipView^>(outputFrame->FindName("FlipView1"));
}

#pragma endregion

#pragma region Sample click handlers - modify if you need them, otherwise delete
void ScenarioInput2::ApplyFont_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Button^ b = dynamic_cast<Button^>(sender);
    if (b != nullptr)
    {
        rootPage->NotifyUser("You clicked the " + b->Name + " button", NotifyType::StatusMessage);
    }

    auto languageFontGroup = ref new LanguageFontGroup("hi");
    
    Page^ outputFrame = dynamic_cast<Page^>(rootPage->OutputFrame->Content);
    auto headingUI = dynamic_cast<TextBlock^>(outputFrame->FindName("Scenario2Heading"));
    auto textUI = dynamic_cast<TextBlock^>(outputFrame->FindName("Scenario2Text"));
            
    if (_oriHeadingUI == nullptr)
    {
        // Store original font style for Reset
        _oriHeadingUI = ref new FontInfoLocal();
        _oriTextUI = ref new FontInfoLocal();
        _oriHeadingUI->Set(headingUI);
        _oriTextUI->Set(textUI);
    }
            
    // Change the Font value with selected font from LanguageFontGroup API.
    FontInfoLocal::SetFont(headingUI, languageFontGroup->DocumentHeadingFont);

    // Not all scripts have recommended fonts for "document alternate"
    // categories, so need to verify before using it. Note that Hindi does
    // have document alternate fonts so in this case the fallback logic is
    // unnecessary, but (for example) Japanese does not have recommendations
    // for the document alternate 2 category.
    if (languageFontGroup->DocumentAlternate2Font != nullptr)
    {
        FontInfoLocal::SetFont(textUI, languageFontGroup->DocumentAlternate2Font);
    }
    else if (languageFontGroup->DocumentAlternate1Font != nullptr)
    {
        FontInfoLocal::SetFont(textUI, languageFontGroup->DocumentAlternate1Font);
    }
    else
    {
        FontInfoLocal::SetFont(textUI, languageFontGroup->ModernDocumentFont);
    }
}

#pragma endregion
