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

#pragma once

#include "Scenario9.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario9 sealed
    {
    public:
        Scenario9();
    protected:
        void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;
    private:
        void InkPresenter_StrokesCollected(Windows::UI::Input::Inking::InkPresenter^ sender, Windows::UI::Input::Inking::InkStrokesCollectedEventArgs^ e);
        void OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
        void OnReset(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnReplay(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void InkReplayTimer_Tick(Platform::Object^ sender, Platform::Object^ e);

        void StopReplay();
        Windows::UI::Input::Inking::InkStrokeContainer^ GetCurrentStrokesView(Windows::Foundation::DateTime time);
        Windows::UI::Input::Inking::InkStroke^ GetPartialStroke(Windows::UI::Input::Inking::InkStroke^ stroke, Windows::Foundation::DateTime time);

        MainPage^ rootPage = MainPage::Current;

        static const int FPS = 60;

        Windows::Foundation::DateTime beginTimeOfRecordedSession;
        Windows::Foundation::DateTime endTimeOfRecordedSession;
        Windows::Foundation::TimeSpan durationOfRecordedSession;
        Windows::Foundation::DateTime beginTimeOfReplay;

        Windows::UI::Xaml::DispatcherTimer^ inkReplayTimer;

        Windows::UI::Input::Inking::InkStrokeBuilder^ strokeBuilder;
        Windows::Foundation::Collections::IVectorView<Windows::UI::Input::Inking::InkStroke^>^ strokesToReplay;
    };
}
