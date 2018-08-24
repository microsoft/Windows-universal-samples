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
#include "Scenario1_UnspecifiedReason.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::ApplicationModel::ExtendedExecution;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

UnspecifiedReason::UnspecifiedReason()
{
    InitializeComponent();
}

void UnspecifiedReason::OnNavigatingFrom(NavigatingCancelEventArgs^ e)
{
    ClearExtendedExecution();
}

void UnspecifiedReason::ClearExtendedExecution()
{
    if (session)
    {
        session->Revoked -= sessionRevokedToken;
        delete session;
        session = nullptr;
    }

    if (periodicTimer)
    {
        periodicTimer->Cancel();
        periodicTimer = nullptr;
    }
}

void UnspecifiedReason::UpdateUI()
{
    if (session == nullptr)
    {
        Status->Text = "Not requested";
        RequestButton->IsEnabled = true;
        CloseButton->IsEnabled = false;
    }
    else
    {
        Status->Text = "Requested";
        RequestButton->IsEnabled = false;
        CloseButton->IsEnabled = true;
    }
}

void UnspecifiedReason::BeginExtendedExecution()
{
    // The previous Extended Execution must be closed before a new one can be requested.
    // This code is redundant here because the sample doesn't allow a new extended
    // execution to begin until the previous one ends, but we leave it here for illustration.
    ClearExtendedExecution();

    auto newSession = ref new ExtendedExecutionSession();
    newSession->Reason = ExtendedExecutionReason::Unspecified;
    newSession->Description = "Raising periodic toasts";
    sessionRevokedToken = newSession->Revoked += ref new TypedEventHandler<Object^, ExtendedExecutionRevokedEventArgs^>(
        this, &UnspecifiedReason::SessionRevoked);
    create_task(newSession->RequestExtensionAsync()).then([this, newSession](ExtendedExecutionResult result)
    {
        switch (result)
        {
        case ExtendedExecutionResult::Allowed:
            rootPage->NotifyUser("Extended execution allowed.", NotifyType::StatusMessage);
            session = newSession;
            startTime = time(nullptr);
            periodicTimer = ThreadPoolTimer::CreatePeriodicTimer(ref new TimerElapsedHandler(this, &UnspecifiedReason::OnTimer), Helpers::TimeSpanFromSeconds(10));
            break;

        default:
        case ExtendedExecutionResult::Denied:
            rootPage->NotifyUser("Extended execution denied.", NotifyType::ErrorMessage);
            delete newSession;
            break;
        }
        UpdateUI();
    }, task_continuation_context::get_current_winrt_context());
}

void UnspecifiedReason::OnTimer(ThreadPoolTimer^ timer)
{
    time_t now = time(nullptr);
    int runningTime = (int)(difftime(now, startTime) + 0.5);
    MainPage::DisplayToast("Extended execution has been active for " + runningTime.ToString() + " seconds");
}

void UnspecifiedReason::EndExtendedExecution()
{
    ClearExtendedExecution();
    UpdateUI();
}

void UnspecifiedReason::SessionRevoked(Object^ sender, ExtendedExecutionRevokedEventArgs^ args)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        switch (args->Reason)
        {
        case ExtendedExecutionRevokedReason::Resumed:
            rootPage->NotifyUser("Extended execution revoked due to returning to foreground.", NotifyType::StatusMessage);
            break;

        case ExtendedExecutionRevokedReason::SystemPolicy:
            rootPage->NotifyUser("Extended execution revoked due to system policy.", NotifyType::StatusMessage);
            break;
        }

        EndExtendedExecution();
    }));
}
