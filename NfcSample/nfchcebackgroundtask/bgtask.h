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

#include "Utilities.h"

using namespace Windows::Storage::Streams;
using namespace Windows::Devices::SmartCards;

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
        typedef enum _DoLaunchType
        {
            Complete,
            Failed,
            Denied
        } DoLaunchType;
        void HandleHceActivation();
        void HandleRegistrationChange();
        void DoLaunch(DoLaunchType type, LPWSTR wszMessage);
        void EndTask();
		void ApduReceived(
			SmartCardEmulator^ emulator,
			SmartCardEmulatorApduReceivedEventArgs^ eventArgs);

		IBuffer^ ProcessCommandApdu(_In_ IBuffer^ commandApdu, _Out_ bool *pfComplete);

		Platform::Guid m_currentConnectionId;
		LPBYTE m_pbCurrentAppletId = nullptr;
        bool m_fTransactionCompleted = false;
        bool m_fDenyTransactions = false;

        void DebugLog(const wchar_t* pwstrMessage);
        void FlushDebugLog();
        Microsoft::WRL::Wrappers::CriticalSection m_csDebugLog;
        std::wstring m_wsDebugLog;

        Microsoft::WRL::Wrappers::SRWLock m_srwLock;
		Windows::ApplicationModel::Background::IBackgroundTaskInstance^ m_taskInstance;
		Platform::Agile<Windows::ApplicationModel::Background::BackgroundTaskDeferral> m_deferral;
		Windows::Devices::SmartCards::SmartCardEmulator^ m_emulator;
        Windows::Devices::SmartCards::SmartCardTriggerDetails^ m_triggerDetails;
        void OnCanceled(Windows::ApplicationModel::Background::IBackgroundTaskInstance ^sender, Windows::ApplicationModel::Background::BackgroundTaskCancellationReason reason);
    };
}
