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
#include "BgTask.h"
#include "Utilities.h"
#include <ppltasks.h>

using namespace NfcHceBackgroundTask;

using namespace Platform;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Foundation;
using namespace concurrency;
using namespace Windows::UI::Notifications;

static Array<byte>^ R_APDU_PPSE_SELECT =
{
	0x6F, 0x23, // File control information (FCI) issuer discretionary data, length
	0x84, 0x0E, // Dedicated file (DF) name, length
	'2', 'P', 'A', 'Y', '.', 'S', 'Y', 'S', '.', 'D', 'D', 'F', '0', '1', // file name "2PAY.SYS.DDF01"
	0xA5, 0x11, // File control information (FCI) proprietary data, length
	0xBF, 0x0C, 0x0E, // File control information issuer discretionary data, length
	0x61, 0x0C, // Directory entry, length
	0x4F, 0x07, // Application definition file (ADF) name, length
	0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10, // Visa credit/debit applet "A0000000031010"   
	0x87, 0x01, // Application priority indicator, length
	0x01,
	0x90, // Status byte one (SW1)
	0x00  // Status byte two (SW2)
};

// EMV_v4.3_Book1_ICC_to_Terminal_Interface, Table 43, 44, 45
static Array<byte>^ R_APDU_CARD_SELECT =
{
	0x6F, 0x1E, // File control information (FCI) issuer discretionary data, length
	0x84, 0x07, // Dedicated file (DF) name, length
							// File name "A0000000031010"
							0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10,
							0xA5, 0x13, // File control information (FCI) proprietary data, length
							0x50, 0x0B, // Application label, length
													// Visa credit/debit application label "CREDIT CARD"
													'C', 'R', 'E', 'D', 'I', 'T', ' ',
													'C', 'A', 'R', 'D',
													0x9F, 0x38, 0x03, // Processing options data object list (PDOL), length
													0x9F, 0x66, 0x02,
													0x90, // Status byte one (SW1)
													0x00  // Status byte two (SW2)
};

// EMV_v4.3_Book3_Application_Specification, Section 6.5.8 and Figure 4
static Array<byte>^ R_APDU_GPO =
{
	0x80, // Class byte (CLA)
	0x06, // Length of Processing options data object list (PDOL)
				// Application interchange profile (AIP), application file locator (AFL)
				0x00, 0x80, 0x08, 0x01, 0x01, 0x00,
				0x90, // Status byte one (SW1)
				0x00  // Status byte two (SW2)
};

/*static Array<byte>^ R_APDU_READ_RECORD_DEFAULT =
{
    0x70, 0x30,
    0x57, 0x12, // Track 2 equivalent data
    0x42, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x2D, 0x22, 0x01, 0x12, 0x10, 0x00, 0x00, 0x99, 0x99, 0x23, 0x4F,
    0x5F, 0x20, 0x0F, // Cardholder name
    0x43, 0x41, 0x52, 0x44, 0x48, 0x4F, 0x4C, 0x44, 0x45, 0x52, 0x2F, 0x4E, 0x41, 0x4D, 0x45,
    0x9F, 0x1F, 0x07, // Track 1 discretionary data
    0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x00,
    0x90, 00
};*/

static Array<byte>^ R_APDU_READ_RECORD_DEFAULT =
{
    0x70, 59, 
        0x57, 0x12, // Track 2 equivalent data
            0x42, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x2D, 0x22, 0x01, 0x12, 0x10, 0x00, 0x00, 0x99, 0x99, 0x23, 0x4F, 
        0x5F, 0x20, 0x0F, // Cardholder name
            0x43, 0x41, 0x52, 0x44, 0x48, 0x4F, 0x4C, 0x44, 0x45, 0x52, 0x2F, 0x4E, 0x41, 0x4D, 0x45, 
        0x9F, 0x1F, 18, // Track 1 discretionary data
            0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35,
    0x90, 00
};

static Array<byte>^ R_APDU_FAIL =
{
	0x6F, // Status byte one (SW1)
	0x00  // Status byte two (SW2)
};

static Array<byte>^ R_APDU_INVALID_PARAMETER_FILE_NOT_FOUND =
{
	0x6A, // Status byte one (SW1)
	0x82  // Status byte two (SW2)
};

BgTask::BgTask() :
	m_taskInstance(nullptr),
	m_deferral(nullptr),
	m_emulator(nullptr)
{
}

void BgTask::Run(
    IBackgroundTaskInstance^ taskInstance)
{
    m_triggerDetails = static_cast<SmartCardTriggerDetails^>(taskInstance->TriggerDetails);
    if (m_triggerDetails == nullptr)
    {
        // May be not a smart card event that triggered us
        return;
    }

    m_emulator = m_triggerDetails->Emulator;
    m_taskInstance = taskInstance;

    switch (m_triggerDetails->TriggerType)
    {
    case SmartCardTriggerType::EmulatorHostApplicationActivated:
        HandleHceActivation();
        break;

    case SmartCardTriggerType::EmulatorAppletIdGroupRegistrationChanged:
        HandleRegistrationChange();
        break;

    default:
        break;
    }

}

void BgTask::HandleRegistrationChange()
{
    auto toastXml = ToastNotificationManager::GetTemplateContent(ToastTemplateType::ToastImageAndText01);
    toastXml->GetElementsByTagName("text")->Item(0)->InnerText = "Registration Changed!";
    auto toast = ref new ToastNotification(toastXml);
    ToastNotificationManager::CreateToastNotifier()->Show(toast);

    return;
}

void BgTask::HandleHceActivation()
{
	try
	{
        auto lock = m_srwLock.LockShared();
        // Take a deferral to keep this background task alive even after this "Run" method returns
        // You must complete this deferal immediately after you have done processing the current transaction
        m_deferral = m_taskInstance->GetDeferral();

        DebugLog(L"*** HCE Activation Background Task Started ***");

        // Set up a handler for if the background task is cancelled, we must immediately complete our deferral
        m_taskInstance->Canceled += ref new Windows::ApplicationModel::Background::BackgroundTaskCanceledEventHandler(
            [this](
            IBackgroundTaskInstance^ sender,
            BackgroundTaskCancellationReason reason)
        {
            DebugLog(L"Cancelled");
            DebugLog(reason.ToString()->Data());
            EndTask();
        });

        if (Windows::Phone::System::SystemProtection::ScreenLocked)
        {
            auto denyIfLocked = Windows::Storage::ApplicationData::Current->RoamingSettings->Values->Lookup("DenyIfPhoneLocked");
            if (denyIfLocked != nullptr && (bool)denyIfLocked == true)
            {
                // The phone is locked, and our current user setting is to deny transactions while locked so let the user know
                // Denied
                DoLaunch(Denied, L"Phone was locked at the time of tap");

                // We still need to respond to APDUs in a timely manner, even though we will just return failure
                m_fDenyTransactions = true;
            }
        }
        else
        {
            m_fDenyTransactions = false;
        }

        m_emulator->ApduReceived += ref new TypedEventHandler<SmartCardEmulator^, SmartCardEmulatorApduReceivedEventArgs^>(
            this, &BgTask::ApduReceived);

        m_emulator->ConnectionDeactivated += ref new TypedEventHandler<SmartCardEmulator^, SmartCardEmulatorConnectionDeactivatedEventArgs^>(
                [this](
                SmartCardEmulator^ emulator,
                SmartCardEmulatorConnectionDeactivatedEventArgs^ eventArgs)
            {
                DebugLog(L"Connection deactivated");
                EndTask();
            });

		m_emulator->Start();
        DebugLog(L"Emulator started");
	}
	catch (Exception^ e)
	{
        DebugLog(("Exception in Run: " + e->ToString())->Data());
        EndTask();
	}
}

void BgTask::DoLaunch(DoLaunchType type, LPWSTR wszMessage)
{
    SmartCardLaunchBehavior launchBehavior = SmartCardLaunchBehavior::Default;
    auto launchAboveLock = Windows::Storage::ApplicationData::Current->RoamingSettings->Values->Lookup("LaunchAboveLock");
    if (launchAboveLock != nullptr && (bool)launchAboveLock == true)
    {
        launchBehavior = SmartCardLaunchBehavior::AboveLock;
    }

    auto args = ref new Platform::String(L"Receipt^");
    switch (type)
    {
    case Complete:
        args += ref new Platform::String(L"Complete^");
        break;
    case Failed:
        args += ref new Platform::String(L"Failed^");
        break;
    case Denied:
        args += ref new Platform::String(L"Denied^");
        break;
    }

    args += ref new Platform::String(wszMessage);

    create_task(m_triggerDetails->TryLaunchCurrentAppAsync(args, launchBehavior)).wait();
}

void BgTask::EndTask()
{
    // Lock exclusive to ensure that all other threads have finished, and no others start doing any work
    DebugLog(L"Waiting for exclusive");
    auto lock = m_srwLock.LockExclusive();
    DebugLog(L"Got exclusive");

    if (!m_fDenyTransactions && !m_fTransactionCompleted)
    {
        // We have haven't already launched the app because of denial/completion, then something went wrong

        DoLaunch(Failed, L"Try tapping again, and hold your phone longer");
    }

    FlushDebugLog();

    if (m_deferral.Get() != nullptr)
    {
        m_deferral->Complete();
    }
}

void BgTask::ApduReceived(SmartCardEmulator^ emulator, SmartCardEmulatorApduReceivedEventArgs^ eventArgs)
{
	try
	{
        auto lock = m_srwLock.LockShared();

		auto connectionId = eventArgs->ConnectionProperties->Id;
		if (connectionId != m_currentConnectionId)
		{
			// This is a new connection, so clear out any state from the previous one
			m_currentConnectionId = connectionId;
			m_pbCurrentAppletId = nullptr;
            m_fTransactionCompleted = false;
            DebugLog(L"New connection");
		}

        bool fTransactionComplete = false;
        IBuffer^ response;

        if (m_fDenyTransactions)
        {
            response = IBufferFromArray(R_APDU_FAIL);
        }
        else
        {
            response = ProcessCommandApdu(eventArgs->CommandApdu, &fTransactionComplete);
        }

		create_task(eventArgs->TryRespondAsync(response)).then(
            [this, fTransactionComplete](bool result)
			{
                DebugLog(result ? L"+Responded successfully" : L"!TryRespondAsync returned false");
                if (result && fTransactionComplete)
                {
                    DoLaunch(Complete, L"");
                    m_fTransactionCompleted = true;
                }
        }).wait();
	}
	catch (Exception^ e)
	{
        DebugLog(("Exception in ApduReceived: " + e->ToString() + "\r\n")->Data());
        DoLaunch(Failed, L"Exception Occured");
        EndTask();
	}
}

#define CLAINS_SELECT 0x00A4
#define CLAINS_GET_PROCESSING_OPTIONS 0x80A8
#define CLAINS_READ_RECORD 0x00B2

BYTE AID_PPSE[] = { '2', 'P', 'A', 'Y', '.', 'S', 'Y', 'S', '.', 'D', 'D', 'F', '0', '1' };
BYTE AID_CARD[] = { 0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10 };

IBuffer^ BgTask::ProcessCommandApdu(IBuffer^ commandApdu, bool *pfComplete)
{
	DWORD cbCommandApdu = 0;
	LPBYTE pbCommandApdu = PointerFromIBuffer(commandApdu, &cbCommandApdu);
	USHORT usClaIns = 0;
    BYTE bP1 = pbCommandApdu[2];
    BYTE bP2 = pbCommandApdu[3];
    LPBYTE pbPayload = nullptr;
    DWORD cbPayload = 0;
    DWORD cbLE = 0;

    *pfComplete = false;

    if (FAILED(ParseApdu(pbCommandApdu, cbCommandApdu, &usClaIns, &bP1, &bP2, &pbPayload, &cbPayload, &cbLE)))
    {
        DebugLog(L"Failed to parse APDU");
        return IBufferFromArray(R_APDU_FAIL);
    }

	switch (usClaIns)
	{
	case CLAINS_SELECT:
        DebugLog(L"-SELECT APDU received");
		if (bP1 != 0x04 || bP2 != 0x00)
		{
			// Unsupported options
            DebugLog(L"SELECT failed with unsupported options");
			return IBufferFromArray(R_APDU_FAIL);
		}

		if (cbPayload == sizeof(AID_PPSE) && memcmp(AID_PPSE, pbPayload, cbPayload) == 0)
		{
			// PPSE selected
			if (cbLE < R_APDU_PPSE_SELECT->Length)
			{
				BYTE rgbResp[] = { 0x6C, (BYTE)R_APDU_PPSE_SELECT->Length };
				return IBufferFromPointer(rgbResp, sizeof(rgbResp));
			}
			else
			{
				m_pbCurrentAppletId = AID_PPSE;
				return IBufferFromArray(R_APDU_PPSE_SELECT);
			}
		}
		else if (cbPayload == sizeof(AID_CARD) && memcmp(AID_CARD, pbPayload, cbPayload) == 0)
		{
			// Card selected
			if (cbLE < R_APDU_CARD_SELECT->Length)
			{
				BYTE rgbResp[] = { 0x6C, (BYTE)R_APDU_CARD_SELECT->Length };
				return IBufferFromPointer(rgbResp, sizeof(rgbResp));
			}
			else
			{
				m_pbCurrentAppletId = AID_CARD;
				return IBufferFromArray(R_APDU_CARD_SELECT);
			}
		}
		else
		{
            DebugLog(L"Unknown AID selected");
			// Invalid applet ID (6A82)
			return IBufferFromArray(R_APDU_INVALID_PARAMETER_FILE_NOT_FOUND);
		}
		break;

	case CLAINS_GET_PROCESSING_OPTIONS:
        DebugLog(L"-GPO APDU received");
		if (bP1 != 0 || bP2 != 0 || m_pbCurrentAppletId != AID_CARD)
		{
            DebugLog(L"GPO failed with unsupported options or mismatched AID");
			return IBufferFromArray(R_APDU_FAIL);
		}
		if (cbLE < R_APDU_GPO->Length)
		{
			BYTE rgbResp[] = { 0x6C, (BYTE)R_APDU_GPO->Length };
			return IBufferFromPointer(rgbResp, sizeof(rgbResp));
		}
		else
		{
			return IBufferFromArray(R_APDU_GPO);
		}
		break;

	case CLAINS_READ_RECORD:
        DebugLog(L"-READ RECORD APDU received");
		if (bP1 != 0x01 || bP2 != 0x0c || m_pbCurrentAppletId != AID_CARD)
		{
            DebugLog(L"READ RECORD failed with unsupported options or mismatched AID");
			return IBufferFromArray(R_APDU_FAIL);
		}

        {
            auto filename = ref new Platform::String(L"ReadRecordResponse-");

            // Find the currently enabled applet ID group in order to get the ID of the read record response file
            auto regs = create_task(SmartCardEmulator::GetAppletIdGroupRegistrationsAsync()).get();
            for (auto iter = regs->First(); iter->HasCurrent; iter->MoveNext())
            {
                if (iter->Current->ActivationPolicy != SmartCardAppletIdGroupActivationPolicy::Disabled
                    && iter->Current->AppletIdGroup->SmartCardEmulationCategory == SmartCardEmulationCategory::Payment)
                {
                    DebugLog(L"Found enabled payment applet ID group");
                    // Found our enabled payment card, add the ID into the filename
                    filename += iter->Current->Id.ToString();
                    break;
                }
            }
            filename += ref new Platform::String(L".dat");
            DebugLog(filename->Data());

            auto readFile = ReadAndUnprotectFileAsync(filename);
            readFile.wait();
            auto fileData = readFile.get();

            // If there's no file, use some default data
            if (fileData == nullptr)
            {
                DebugLog(L"No READ RECORD file found, using default data");
                fileData = R_APDU_READ_RECORD_DEFAULT;
            }
            else
            {
                DebugLog(L"File found for READ RECORD APDU response");
            }

            *pfComplete = true;
            return IBufferFromArray(fileData);
        }
		break;

	default:
        DebugLog(L"-Unknown APDU received");
		return IBufferFromArray(R_APDU_FAIL);
	}

    DebugLog(L"Fallthrough failure in ProcessCommandApdu");
	return IBufferFromArray(R_APDU_FAIL);
}

void BgTask::DebugLog(const wchar_t* pwstrMessage)
{
    auto lock = m_csDebugLog.Lock();
    m_wsDebugLog += pwstrMessage;
    m_wsDebugLog += L"\r\n";
}

void BgTask::FlushDebugLog()
{
    auto debugLogLock = m_csDebugLog.Lock();
    AppendFile(L"DebugLog.txt", ref new Platform::String(m_wsDebugLog.data()));
    debugLogLock.Unlock();
}


