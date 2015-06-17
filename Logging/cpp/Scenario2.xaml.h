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

#include "Scenario2.g.h"
#include "LoggingSessionScenario.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2 sealed
    {
    public:

        Scenario2();

    protected:

        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:

        property LoggingSessionScenario^ LoggingScenario
        {
            LoggingSessionScenario^ get()
            {
                return LoggingSessionScenario::Instance::get();
            }
        }

        concurrency::task<void> UpdateLogMessageCountDispatchAsync();
        void AddMessage(Platform::String^ message);
        concurrency::task<void> AddMessageDispatch(Platform::String^ message);
        concurrency::task<void> AddLogFileMessageDispatch(Platform::String^ message, const std::wstring& logFileFullPath);
        void UpdateControls();
        void DoScenario(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void EnableDisableLogging(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnStatusChanged(Platform::Object^ sender, LoggingScenarioEventArgs^ args);
        Windows::UI::Xaml::Controls::ScrollViewer^ FindScrollViewer(DependencyObject^ depObject);
    };
}
