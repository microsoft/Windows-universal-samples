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
#include "Scenario3_ShowHideMethods.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Navigation;

Scenario3_ShowHideMethods::Scenario3_ShowHideMethods()
{
    InitializeComponent();
}

void Scenario3_ShowHideMethods::OnNavigatedFrom(NavigationEventArgs^ e)
{
    FadeOutResults->Stop();
}

void Scenario3_ShowHideMethods::WordListBox_OnKeyUp(Object^ /*sender*/, KeyRoutedEventArgs^ e)
{
    if (e->Key == Windows::System::VirtualKey::Enter)
    {
        String^ text = WordListBox->Text;
        if (!text->IsEmpty())
        {
            InputPane::GetForCurrentView()->TryHide();
            ResultsTextBlock->Text = text;
            FadeOutResults->Begin();
        }
    }
}

void Scenario3_ShowHideMethods::OnFadeOutCompleted(Object^ /*sender*/, Object^ /*e*/)
{
    ResultsTextBlock->Text = L"";
    InputPane::GetForCurrentView()->TryShow();
}
