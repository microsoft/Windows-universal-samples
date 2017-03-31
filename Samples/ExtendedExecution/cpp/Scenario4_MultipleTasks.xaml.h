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

#include "Scenario4_MultipleTasks.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// Page containing the Extended Execution LocationTracking Reason scenario functions.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class MultipleTasks sealed
    {
    public:
        MultipleTasks();

    protected:
        void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;
        void BeginExtendedExecution();
        void EndExtendedExecution();

    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::ApplicationModel::ExtendedExecution::ExtendedExecutionSession^ session;
        Concurrency::cancellation_token_source cancellationTokenSource;
        Windows::Foundation::EventRegistrationToken sessionRevokedToken;
        int taskCount = 0;

        void ClearExtendedExecution();
        void UpdateUI();
        void OnTaskCompleted();
        Windows::Foundation::Deferral^ GetExecutionDeferral();
        void RaiseToastAfterDelay(int id, int seconds);
        void SessionRevoked(Platform::Object^ sender, Windows::ApplicationModel::ExtendedExecution::ExtendedExecutionRevokedEventArgs^ args);
    };
}
