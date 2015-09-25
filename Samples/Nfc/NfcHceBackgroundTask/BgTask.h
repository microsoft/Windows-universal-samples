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

namespace NfcHceBackgroundTask
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class BgTask sealed :
        public Windows::ApplicationModel::Background::IBackgroundTask
    {
    public:
        BgTask();

        virtual void Run(
            Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);

    private:
        typedef enum _LaunchType
        {
            Complete,
            Failed,
            Denied
        } LaunchType;

    private:
        void HandleHceActivation();

        void LaunchForegroundApp(LaunchType type, LPWSTR wszMessage);

        void EndTask();

        void ApduReceived(
            Windows::Devices::SmartCards::SmartCardEmulator^ emulator,
            Windows::Devices::SmartCards::SmartCardEmulatorApduReceivedEventArgs^ eventArgs);

        Windows::Storage::Streams::IBuffer^ ProcessCommandApdu(
            _In_ Windows::Storage::Streams::IBuffer^ commandApdu,
            _Out_ bool* pfComplete);

        void DebugLogString(Platform::String^ message);

        void DebugLog(const wchar_t* pwstrMessage);

        void FlushDebugLog();

    private:
        Microsoft::WRL::Wrappers::CriticalSection m_csLock;
        Windows::ApplicationModel::Background::IBackgroundTaskInstance^ m_taskInstance;
        Platform::Agile<Windows::ApplicationModel::Background::BackgroundTaskDeferral> m_deferral;
        Windows::Devices::SmartCards::SmartCardEmulator^ m_emulator;
        Windows::Devices::SmartCards::SmartCardTriggerDetails^ m_triggerDetails;
        Windows::Devices::SmartCards::SmartCardAppletIdGroupRegistration^ m_paymentAidRegistration;

        Platform::Guid m_currentConnectionId;
        LPBYTE m_pbCurrentAppletId = nullptr;
        bool m_fTransactionCompleted = false;
        bool m_fDenyTransactions = false;
        bool m_fTaskEnded = false;

        Microsoft::WRL::Wrappers::CriticalSection m_csDebugLog;
        std::wstring m_wsDebugLog;
    };
}
