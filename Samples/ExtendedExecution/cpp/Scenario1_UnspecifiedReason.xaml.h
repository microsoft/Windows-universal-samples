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

#include "Scenario1_UnspecifiedReason.g.h"
#include "MainPage.xaml.h"
#include <time.h>

namespace SDKTemplate
{
    /// <summary>
    /// Page containing the Extended Execution Unspecified Reason scenario functions.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class UnspecifiedReason sealed
    {
    public:
        UnspecifiedReason();

    protected:
        void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;
        void BeginExtendedExecution();
        void EndExtendedExecution();

    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::ApplicationModel::ExtendedExecution::ExtendedExecutionSession^ session;
        Windows::System::Threading::ThreadPoolTimer^ periodicTimer;
        Windows::Foundation::EventRegistrationToken sessionRevokedToken;
        time_t startTime;

        void ClearExtendedExecution();
        void UpdateUI();
        void OnTimer(Windows::System::Threading::ThreadPoolTimer^ timer);
        void SessionRevoked(Platform::Object^ sender, Windows::ApplicationModel::ExtendedExecution::ExtendedExecutionRevokedEventArgs^ args);

    };
}
