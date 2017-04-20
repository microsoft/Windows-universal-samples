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
#include "Scenario2_SavingDataReason.xaml.h"

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

SavingDataReason::SavingDataReason()
{
    InitializeComponent();
}

void SavingDataReason::OnNavigatedTo(NavigationEventArgs^ e)
{
    // Add a suspension handler in order to request a SavingData Extended Execution.
    suspendingToken = App::Current->Suspending += ref new SuspendingEventHandler(this, &SavingDataReason::OnSuspending);
}

void SavingDataReason::OnNavigatingFrom(NavigatingCancelEventArgs^ e)
{
    App::Current->Suspending -= suspendingToken;
}

void SavingDataReason::OnSuspending(Object^ sender, SuspendingEventArgs^ args)
{
    SuspendingDeferral^ suspendDeferral = args->SuspendingOperation->GetDeferral();

    rootPage->NotifyUser("", NotifyType::StatusMessage);

    auto session = ref new ExtendedExecutionSession();

    session->Reason = ExtendedExecutionReason::SavingData;
    session->Description = "Pretending to save data to slow storage.";
    EventRegistrationToken revokedToken = session->Revoked += ref new TypedEventHandler<Object^, ExtendedExecutionRevokedEventArgs^>(this, &SavingDataReason::ExtendedExecutionSessionRevoked);
    create_task(session->RequestExtensionAsync()).then([this](ExtendedExecutionResult result)
    {
        switch (result)
        {
        case ExtendedExecutionResult::Allowed:
            // We can perform a longer save operation (e.g., upload to the cloud).
            MainPage::DisplayToast("Performing a long save operation.");
            cancellationTokenSource = cancellation_token_source();
            return Helpers::DelayAsync(Helpers::TimeSpanFromSeconds(10), cancellationTokenSource.get_token())
                .then([this]()
            {
                MainPage::DisplayToast("Still saving.");
                return Helpers::DelayAsync(Helpers::TimeSpanFromSeconds(10), cancellationTokenSource.get_token());
            }).then([this]()
            {
                MainPage::DisplayToast("Long save complete.");
            }).then([this](task<void> previousTask)
            {
                try
                {
                    // Raise the cancellation exception if the task was canceled.
                    previousTask.get();
                }
                catch (const task_canceled&)
                {
                    // The extended execution was revoked before we could finish the long save operation.
                }
            });
            break;

        default:
        case ExtendedExecutionResult::Denied:
            // We must perform a fast save operation.
            MainPage::DisplayToast("Performing a fast save operation.");
            return Helpers::DelayAsync(Helpers::TimeSpanFromSeconds(1)).then([this]()
            {
                MainPage::DisplayToast("Fast save complete.");
            });
            break;
        }
    }).then([this, session, revokedToken, suspendDeferral]()
    {
        session->Revoked -= revokedToken;
        delete session;
        suspendDeferral->Complete();
    });
}

void SavingDataReason::ExtendedExecutionSessionRevoked(Object^ sender, ExtendedExecutionRevokedEventArgs^ args)
{
    // If session is revoked, make the OnSuspending event handler stop or the application will be terminated.
    cancellationTokenSource.cancel();

    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        switch (args->Reason)
        {
        case ExtendedExecutionRevokedReason::Resumed:
            // A resumed app has returned to the foreground
            rootPage->NotifyUser("Extended execution revoked due to returning to foreground.", NotifyType::StatusMessage);
            break;

        case ExtendedExecutionRevokedReason::SystemPolicy:
            // An app can be in the foreground or background when a revocation due to system policy occurs
            MainPage::DisplayToast("Extended execution revoked due to system policy.");
            rootPage->NotifyUser("Extended execution revoked due to system policy.", NotifyType::StatusMessage);
            break;
        }
    }));
}
