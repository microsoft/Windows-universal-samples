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
#include "Scenario3_ShowHideMethods.h"
#include "Scenario3_ShowHideMethods.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::ViewManagement;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_ShowHideMethods::Scenario3_ShowHideMethods()
    {
        InitializeComponent();
    }

    void Scenario3_ShowHideMethods::OnNavigatedFrom(NavigationEventArgs const&)
    {
        FadeOutResults().Stop();
    }

    void Scenario3_ShowHideMethods::WordListBox_OnKeyUp(IInspectable const&, KeyRoutedEventArgs const& e)
    {
        if (e.Key() == Windows::System::VirtualKey::Enter)
        {
            hstring text = WordListBox().Text();
            if (!text.empty())
            {
                InputPane::GetForCurrentView().TryHide();
                ResultsTextBlock().Text(text);
                FadeOutResults().Begin();
            }
        }
    }

    void Scenario3_ShowHideMethods::OnFadeOutCompleted(IInspectable const&, IInspectable const&)
    {
        ResultsTextBlock().Text({});
        InputPane::GetForCurrentView().TryShow();
    }

}
