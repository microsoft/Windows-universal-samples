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
#include "Scenario4_MultipleTasks.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::ExtendedExecution;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

MultipleTasks::MultipleTasks()
{
    InitializeComponent();
}

void MultipleTasks::OnNavigatingFrom(NavigatingCancelEventArgs^ e)
{
    // End the Extended Execution Session.
    // Only one extended execution session can be held by an application at one time.
    ClearExtendedExecution();
}

void MultipleTasks::ClearExtendedExecution()
{
    // Cancel any outstanding tasks.
    cancellationTokenSource.cancel();

    // Dispose any outstanding session.
    if (session)
    {
        session->Revoked -= sessionRevokedToken;
        delete session;
        session = nullptr;
    }
}

void MultipleTasks::UpdateUI()
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

void MultipleTasks::OnTaskCompleted()
{
    taskCount--;
    if (taskCount == 0 && session != nullptr)
    {
        EndExtendedExecution();
        MainPage::DisplayToast("All Tasks Completed, ending Extended Execution.");
    }
}

Deferral^ MultipleTasks::GetExecutionDeferral()
{
    if (session == nullptr)
    {
        throw ref new FailureException("No extended execution session is active");
    }

    taskCount++;
    return ref new Deferral(ref new DeferralCompletedHandler(this, &MultipleTasks::OnTaskCompleted));
}

void MultipleTasks::BeginExtendedExecution()
{
    // The previous Extended Execution must be closed before a new one can be requested.
    // This code is redundant here because the sample doesn't allow a new extended
    // execution to begin until the previous one ends, but we leave it here for illustration.
    ClearExtendedExecution();

    auto newSession = ref new ExtendedExecutionSession();
    newSession->Reason = ExtendedExecutionReason::Unspecified;
    newSession->Description = "Running multiple tasks";
    sessionRevokedToken = newSession->Revoked += ref new TypedEventHandler<Object^, ExtendedExecutionRevokedEventArgs^>(
        this, &MultipleTasks::SessionRevoked);
    create_task(newSession->RequestExtensionAsync()).then([this, newSession](ExtendedExecutionResult result)
    {
        switch (result)
        {
        case ExtendedExecutionResult::Allowed:
            rootPage->NotifyUser("Extended execution allowed.", NotifyType::StatusMessage);
            session = newSession;
            break;

        default:
        case ExtendedExecutionResult::Denied:
            rootPage->NotifyUser("Extended execution denied.", NotifyType::ErrorMessage);
            delete newSession;
            break;
        }
        UpdateUI();

        if (session != nullptr)
        {
            cancellationTokenSource = cancellation_token_source();

            // Start up a few tasks that all share this session.
            Deferral^ deferral = GetExecutionDeferral();

            for (int i = 0; i < 3; i++)
            {
                int randomDelay = 5 + rand() % 6;
                RaiseToastAfterDelay(i, randomDelay);
            }

            deferral->Complete();
        }
    }, task_continuation_context::get_current_winrt_context());
}

void MultipleTasks::RaiseToastAfterDelay(int id, int seconds)
{
    Deferral^ deferral = GetExecutionDeferral();

    Helpers::DelayAsync(Helpers::TimeSpanFromSeconds(seconds), cancellationTokenSource.get_token())
        .then([this, id, seconds, deferral](task<void> previousTask)
    {
        try
        {
            // Raise the cancellation exception if the task was canceled.
            previousTask.get();

            // This code runs if the task was not canceled.
            MainPage::DisplayToast("Task " + id.ToString() + " completed after " + seconds.ToString() + " seconds");
        }
        catch (const task_canceled&)
        {
            // This code runs if the task was canceled.
            MainPage::DisplayToast("Task " + id.ToString() + " canceled");
        }

        deferral->Complete();
    }, task_continuation_context::get_current_winrt_context());
}

void MultipleTasks::EndExtendedExecution()
{
    ClearExtendedExecution();
    UpdateUI();
}

void MultipleTasks::SessionRevoked(Object^ sender, ExtendedExecutionRevokedEventArgs^ args)
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
