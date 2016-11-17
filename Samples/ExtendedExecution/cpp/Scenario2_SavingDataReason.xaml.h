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

#include "Scenario2_SavingDataReason.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// Page containing the Extended Execution SavingData Reason scenario functions.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class SavingDataReason sealed
    {
    public:
        SavingDataReason();

    protected:
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;

    private:
        MainPage^ rootPage = MainPage::Current;
        Concurrency::cancellation_token_source cancellationTokenSource;
        Windows::Foundation::EventRegistrationToken suspendingToken;

        void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
        void ExtendedExecutionSessionRevoked(Platform::Object^ sender, Windows::ApplicationModel::ExtendedExecution::ExtendedExecutionRevokedEventArgs^ args);
    };
}
