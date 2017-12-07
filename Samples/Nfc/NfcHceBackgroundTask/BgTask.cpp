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

using namespace NfcHceBackgroundTask;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Devices::SmartCards;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Notifications;

#define CLAINS_SELECT 0x00A4
#define CLAINS_GET_PROCESSING_OPTIONS 0x80A8
#define CLAINS_READ_RECORD 0x00B2
#define CLAINS_COMPUTE_CRYPTO_CSUM 0x802A

BYTE AID_PPSE[] = { '2', 'P', 'A', 'Y', '.', 'S', 'Y', 'S', '.', 'D', 'D', 'F', '0', '1' };
BYTE AID_V[] = { 0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10 };
BYTE AID_MC[] = { 0xA0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10 };
BYTE AID_NONPAYMENT[] = { 0x12, 0x34, 0x56, 0x78, 0x90 };

static Array<byte>^ R_APDU_PPSE_SELECT = nullptr;

static Array<byte>^ R_APDU_PPSE_SELECT_V =
{
    0x6F, 0x23, // File control information (FCI) issuer discretionary data, length
    0x84, 0x0E, // Dedicated file (DF) name, length
    '2', 'P', 'A', 'Y', '.', 'S', 'Y', 'S', '.', 'D', 'D', 'F', '0', '1', // file name "2PAY.SYS.DDF01"
    0xA5, 0x11, // File control information (FCI) proprietary data, length
    0xBF, 0x0C, 0x0E, // File control information issuer discretionary data, length
    0x61, 0x0C, // Directory entry, length
    0x4F, 0x07, // Application definition file (ADF) name, length
    0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10, // V credit/debit applet "A0000000031010"   
    0x87, 0x01, // Application priority indicator, length
    0x01,
    0x90, // Status byte one (SW1)
    0x00  // Status byte two (SW2)
};

static Array<byte>^ R_APDU_PPSE_SELECT_MC =
{
    0x6F, 0x23, // File control information (FCI) issuer discretionary data, length
        0x84, 0x0E, // Dedicated file (DF) name, length
            '2', 'P', 'A', 'Y', '.', 'S', 'Y', 'S', '.', 'D', 'D', 'F', '0', '1', // file name "2PAY.SYS.DDF01"
        0xA5, 0x11, // File control information (FCI) proprietary data, length
            0xBF, 0x0C, 0x0E, // File control information issuer discretionary data, length
                0x61, 0x0C, // Directory entry, length
                    0x4F, 0x07, // Application definition file (ADF) name, length
                        0xA0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10, // MC credit/debit applet "A0000000041010"   
                    0x87, 0x01, // Application priority indicator, length
                        0x01,
    0x90, // Status byte one (SW1)
    0x00  // Status byte two (SW2)
};

// EMV_v4.3_Book1_ICC_to_Terminal_Interface, Table 43, 44, 45
static Array<byte>^ R_APDU_CARD_SELECT_V =
{
    0x6F, 0x1E, // File control information (FCI) issuer discretionary data, length
        0x84, 0x07, // Dedicated file (DF) name, length
            // File name "A0000000031010"
            0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10,
        0xA5, 0x13, // File control information (FCI) proprietary data, length
            0x50, 0x0B, // Application label, length
                // Credit/debit application label "CREDIT CARD"
                'C', 'R', 'E', 'D', 'I', 'T', ' ',
                'C', 'A', 'R', 'D',
            0x9F, 0x38, 0x03, // Processing options data object list (PDOL), length
                0x9F, 0x66, 0x02,
    0x90, // Status byte one (SW1)
    0x00  // Status byte two (SW2)
};

// EMV_v4.3_Book1_ICC_to_Terminal_Interface, Table 43, 44, 45
static Array<byte>^ R_APDU_CARD_SELECT_MC =
{
    0x6F, 0x18, // File control information (FCI) issuer discretionary data, length
        0x84, 0x07, // Dedicated file (DF) name, length
            // File name "A0000000041010"
            0xA0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10,
        0xA5, 0x0D, // File control information (FCI) proprietary data, length
            0x50, 0x0B, // Application label, length
                // Credit/debit application label "CREDIT CARD"
                'C', 'R', 'E', 'D', 'I', 'T', ' ',
                'C', 'A', 'R', 'D',
    0x90, // Status byte one (SW1)
    0x00  // Status byte two (SW2)
};

// EMV_v4.3_Book3_Application_Specification, Section 6.5.8 and Figure 4
static Array<byte>^ R_APDU_GPO_V =
{
    0x80,
    0x06, // Length of Processing options data object list (PDOL)
                // Application interchange profile (AIP), application file locator (AFL)
                0x00, 0x80, 0x08, 0x01, 0x01, 0x00,
                0x90, // Status byte one (SW1)
                0x00  // Status byte two (SW2)
};

static Array<byte>^ R_APDU_GPO_MC =
{
    0x77, 0x0F, // Response Message Template Format 2
        0x82, 0x02, // Application Interchange Profile
            0x00, 0x00, // Only magstripe supported
        0x94, 0x04, // Application File Locator
            0x08, 0x01, 0x01, 0x00,
        0xD7, 0x03,     // PayPass application control
            0x00, 0x00, 0x80,   // Static CVC3
    0x90, // Status byte one (SW1)
    0x00  // Status byte two (SW2)
};

static Array<byte>^ R_APDU_READ_RECORD_DEFAULT_V =
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

static Array<byte>^ R_APDU_READ_RECORD_DEFAULT_MC =
{
    0x70, 0x81, 0x94, // Application Elementary File(AEF) Template
    0x9F, 0x6C, 0x02, // Mag Stripe Application Version Number(Card)
            0x00, 0x01,

        0x56, 0x42, // Track 1 Data
            0x42, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x34, 0x34, 0x34, 0x34, //  "B5555555555554444"
            0x5E, 0x43, 0x41, 0x52, 0x44, 0x48, 0x4F, 0x4C, 0x44, 0x45, 0x52, 0x2F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // "^CARDHOLDER/     "
            0x5E, 0x31, 0x37, 0x30, 0x34, 0x35, 0x30, 0x32, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, // "^1704502000000000000000"
            0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,

        0x9F, 0x62, 0x06, // Track 1 Bit Map for CVC3(PCVC3 Track 1)
            0x00, 0x00, 0x00, 0x38, 0x00, 0x00,

        0x9F, 0x63, 0x06, // Track 1 Bit Map for UN and ATC(PUNATC Track 1)
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // E0 E0

        0x9F, 0x64, 0x01, // Track 1 Number of ATC Digits(NATC Track 1)
            0x00, //3

        0x9F, 0x6B, 0x13, // Track 2 Data
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x44, 0x44, 0xD1, 0x70, 0x45, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4F,

        0x9F, 0x65, 0x02, // Track 2 Bit Map for CVC3(PCVC3 Track 2)
            0x00, 0x0E,

        0x9F, 0x66, 0x02, // Track 2 Bit Map for UN and ATC(PUNATC Track 2)
            0x00, 0x00, // 0E 70

        0x9F, 0x67, 0x01, // Track 2 Number of ATC Digits(NATC Track 2)
            0x00, //3

        0x9F, 0x68, 0x0E, // Mag Stripe CVM List
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5E, 0x03, 0x42, 0x03, 0x1F, 0x03,

    0x90, 00,
};

static Array<byte>^ R_APDU_COMPUTE_CRYPTO_CSUM_MC =
{
    0x77, 0x0F, // Response Message Template Format 2
        0x9F, 0x61, 0x02, // CVC3 Track 2
            0x00, 0x98,

        0x9F, 0x60, 0x02, // CVC3 Track 1
            0x03, 0x0C,

        0x9F, 0x36, 0x02, // Application Transaction Counter(ATC)
            0x00, 0x01,

    0x90, 0x00,
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

static Array<byte>^ R_APDU_SUCCESS =
{
    0x90, // Status byte one (SW1)
    0x00  // Status byte two (SW2)
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
    m_taskInstance = taskInstance;

    m_triggerDetails = static_cast<SmartCardTriggerDetails^>(taskInstance->TriggerDetails);
    m_emulator = m_triggerDetails->Emulator;

    switch (m_triggerDetails->TriggerType)
    {
    case SmartCardTriggerType::EmulatorHostApplicationActivated:
        HandleHceActivation();
        break;

    default:
        DebugLogString(m_triggerDetails->TriggerType.ToString());
        FlushDebugLog();

        // Show toast displaying the type of the background task trigger
        auto toastXml = ToastNotificationManager::GetTemplateContent(ToastTemplateType::ToastImageAndText01);
        toastXml->GetElementsByTagName("text")->Item(0)->InnerText = m_triggerDetails->TriggerType.ToString();
        auto toast = ref new ToastNotification(toastXml);
        ToastNotificationManager::CreateToastNotifier()->Show(toast);

        break;
    }
}

void BgTask::HandleHceActivation()
{
    try
    {
        auto lock = m_csLock.Lock();

        // Take a deferral to keep this background task alive even after this "Run" method returns.
        // You must complete this deferal immediately after you have done processing the current
        // transaction
        m_deferral = m_taskInstance->GetDeferral();

        DebugLog(L"*** HCE Activation Background Task Started ***");

        // Find the currently enabled payment applet ID group
        auto regs = create_task(SmartCardEmulator::GetAppletIdGroupRegistrationsAsync()).get();
        for (auto iter = regs->First(); iter->HasCurrent; iter->MoveNext())
        {
            if (iter->Current->ActivationPolicy != SmartCardAppletIdGroupActivationPolicy::Disabled
                && iter->Current->AppletIdGroup->SmartCardEmulationCategory == SmartCardEmulationCategory::Payment)
            {
                m_paymentAidRegistration = iter->Current;

                // If the activation policy is Enabled, continue searching because there might be
                // an applet ID group with an activation policy of ForegroundOverride
                if (iter->Current->ActivationPolicy == SmartCardAppletIdGroupActivationPolicy::ForegroundOverride)
                {
                    break;
                }
            }
        }

        if (m_paymentAidRegistration != nullptr)
        {
            DebugLogString(L"Found enabled payment applet ID group: " + m_paymentAidRegistration->AppletIdGroup->DisplayName);

            // Check what type of payment card we're handling and set up for it
            auto appletIds = m_paymentAidRegistration->AppletIdGroup->AppletIds;
            for (auto iter = appletIds->First(); iter->HasCurrent; iter->MoveNext())
            {
                DWORD cbLen = 0;
                auto pbAid = PointerFromIBuffer(iter->Current, &cbLen);
                if (cbLen == sizeof(AID_MC) && memcmp(pbAid, AID_MC, cbLen) == 0)
                {
                    // There's a MC AID in the group, so use the MC PPSE
                    R_APDU_PPSE_SELECT = R_APDU_PPSE_SELECT_MC;
                    break;
                }
                else if (cbLen == sizeof(AID_V) && memcmp(pbAid, AID_V, cbLen) == 0)
                {
                    // There's a V AID in the group, so use the V PPSE
                    R_APDU_PPSE_SELECT = R_APDU_PPSE_SELECT_V;
                    break;
                }
            }
        }

        m_fDenyTransactions = false;

        if (Windows::Phone::System::SystemProtection::ScreenLocked)
        {
            auto denyIfLocked = ApplicationData::Current->RoamingSettings->Values->Lookup("DenyIfPhoneLocked");
            if (denyIfLocked != nullptr && (bool)denyIfLocked == true)
            {
                // The phone is locked, and our current user setting is to deny transactions while
                // locked, so deny transactions and let the user know. We still need to respond to
                // APDUs in a timely manner, even though we will just respond with failure APDUs
                m_fDenyTransactions = true;
                LaunchForegroundApp(Denied, L"Phone was locked at the time of tap");
            }
        }

        m_emulator->ApduReceived += ref new TypedEventHandler<SmartCardEmulator^, SmartCardEmulatorApduReceivedEventArgs^>(
            this, &BgTask::ApduReceived);

        // Set up a handler for if the background task is canceled, we must immediately complete
        // our deferral
        m_taskInstance->Canceled += ref new BackgroundTaskCanceledEventHandler(
            [this](
                IBackgroundTaskInstance^ sender,
                BackgroundTaskCancellationReason reason)
            {
                auto lock = m_csLock.Lock();

                DebugLogString(L"Bg task canceled. Reason: " + reason.ToString());
                EndTask();
            });

        m_emulator->ConnectionDeactivated += ref new TypedEventHandler<SmartCardEmulator^, SmartCardEmulatorConnectionDeactivatedEventArgs^>(
            [this](
                SmartCardEmulator^ emulator,
                SmartCardEmulatorConnectionDeactivatedEventArgs^ eventArgs)
            {
                auto lock = m_csLock.Lock();

                DebugLogString(L"Connection deactivated. Reason: " + eventArgs->Reason.ToString());
                EndTask();
            });

        // SmartCardEmulator::Start() can trigger the ConnectionDeactivated and ApduReceived
        // callbacks on this thread. If you are using a lock here and in one of those callbacks,
        // then make sure the lock is reentrant/recursive
        DebugLog(L"Emulator is starting...");
        m_emulator->Start();

        if (m_fTaskEnded)
        {
            DebugLog(L"Connection was deactivated before emulator could start");
            FlushDebugLog();
        }
        else
        {
            DebugLog(L"Emulator started");
        }
    }
    catch (Exception^ e)
    {
        DebugLogString(L"Exception in Run: " + e->ToString());
        EndTask();
    }
}

void BgTask::LaunchForegroundApp(LaunchType type, LPWSTR wszMessage)
{
    SmartCardLaunchBehavior launchBehavior = SmartCardLaunchBehavior::Default;
    auto launchAboveLock = ApplicationData::Current->RoamingSettings->Values->Lookup("LaunchAboveLock");
    if (launchAboveLock != nullptr && (bool)launchAboveLock == true)
    {
        launchBehavior = SmartCardLaunchBehavior::AboveLock;
    }

    auto args = ref new String(L"Receipt^");
    switch (type)
    {
    case Complete:
        args += ref new String(L"Complete^");
        break;
    case Failed:
        args += ref new String(L"Failed^");
        break;
    case Denied:
        args += ref new String(L"Denied^");
        break;
    }

    args += ref new String(wszMessage);

    bool succeeded = create_task(
        m_triggerDetails->TryLaunchCurrentAppAsync(args, launchBehavior)).get();
    if (!succeeded)
    {
        DebugLog(L"Foreground app failed to launch");
    }
}

void BgTask::EndTask()
{
    if (m_fTaskEnded)
    {
        DebugLog(L"Background task has already ended. Leaving EndTask");
        return;
    }

    if (!m_fDenyTransactions)
    {
        if (!m_fTransactionCompleted)
        {
            DebugLog(L"Transaction failed to complete");
            LaunchForegroundApp(Failed, L"Try tapping again, and hold your phone on the reader longer");
        }
        else
        {
            DebugLog(L"Transaction complete");
        }
    }

    FlushDebugLog();

    m_fTaskEnded = true;

    if (m_deferral.Get() != nullptr)
    {
        m_deferral->Complete();
    }
}

void BgTask::ApduReceived(SmartCardEmulator^ emulator, SmartCardEmulatorApduReceivedEventArgs^ eventArgs)
{
    try
    {
        auto lock = m_csLock.Lock();

        if (m_fTaskEnded)
        {
            DebugLog(L"Background task has been canceled. Leaving ApduReceived");
            return;
        }

        auto apduBuffer = eventArgs->CommandApdu;
        auto apduBytes = ref new Array<byte>(apduBuffer->Length);
        auto dataReader = DataReader::FromBuffer(apduBuffer);
        dataReader->ReadBytes(apduBytes);

        auto strApdu = L"Apdu received: " + ByteArrayToString(apduBytes);
        DebugLog(strApdu.c_str());

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
            response = ProcessCommandApdu(apduBuffer, &fTransactionComplete);
        }

        if (eventArgs->AutomaticResponseStatus != SmartCardAutomaticResponseStatus::Success)
        {
            create_task(eventArgs->TryRespondAsync(response)).then(
                [this, fTransactionComplete](bool result)
            {
                auto lock = m_csLock.Lock();

                DebugLog(result ? L"+Responded successfully" : L"!TryRespondAsync returned false");
                if (result && fTransactionComplete)
                {
                    m_fTransactionCompleted = true;
                    LaunchForegroundApp(Complete, L"");
                }
            });
        }
        else
        {
            DebugLog(L"+System auto-responded already");
            if (fTransactionComplete)
            {
                m_fTransactionCompleted = true;
                LaunchForegroundApp(Complete, L"");
            }
        }
    }
    catch (Exception^ e)
    {
        DebugLogString(L"Exception in ApduReceived: " + e->ToString());
        LaunchForegroundApp(Failed, L"Exception occurred");
        EndTask();
    }
}

_Use_decl_annotations_
IBuffer^ BgTask::ProcessCommandApdu(IBuffer^ commandApdu, bool* pfComplete)
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

        if (R_APDU_PPSE_SELECT != nullptr && cbPayload == sizeof(AID_PPSE) && memcmp(AID_PPSE, pbPayload, cbPayload) == 0)
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
        else if (cbPayload == sizeof(AID_V) && memcmp(AID_V, pbPayload, cbPayload) == 0)
        {
            // Card selected
            if (cbLE < R_APDU_CARD_SELECT_V->Length)
            {
                BYTE rgbResp[] = { 0x6C, (BYTE)R_APDU_CARD_SELECT_V->Length };
                return IBufferFromPointer(rgbResp, sizeof(rgbResp));
            }
            else
            {
                m_pbCurrentAppletId = AID_V;
                return IBufferFromArray(R_APDU_CARD_SELECT_V);
            }
        }
        else if (cbPayload == sizeof(AID_MC) && memcmp(AID_MC, pbPayload, cbPayload) == 0)
        {
            // Card selected
            if (cbLE < R_APDU_CARD_SELECT_MC->Length)
            {
                BYTE rgbResp[] = { 0x6C, (BYTE)R_APDU_CARD_SELECT_MC->Length };
                return IBufferFromPointer(rgbResp, sizeof(rgbResp));
            }
            else
            {
                m_pbCurrentAppletId = AID_MC;
                return IBufferFromArray(R_APDU_CARD_SELECT_MC);
            }
        }
        else if (cbPayload == sizeof(AID_NONPAYMENT) && memcmp(AID_NONPAYMENT, pbPayload, cbPayload) == 0)
        {
            return IBufferFromArray(R_APDU_SUCCESS);
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
        if (bP1 != 0 || bP2 != 0)
        {
            DebugLog(L"GPO failed with unsupported options");
            return IBufferFromArray(R_APDU_FAIL);
        }
        if (m_pbCurrentAppletId == AID_V)
        {
            if (cbLE < R_APDU_GPO_V->Length)
            {
                BYTE rgbResp[] = { 0x6C, (BYTE)R_APDU_GPO_V->Length };
                return IBufferFromPointer(rgbResp, sizeof(rgbResp));
            }
            else
            {
                return IBufferFromArray(R_APDU_GPO_V);
            }
        }
        else if (m_pbCurrentAppletId == AID_MC)
        {
            if (cbLE < R_APDU_GPO_MC->Length)
            {
                BYTE rgbResp[] = { 0x6C, (BYTE)R_APDU_GPO_MC->Length };
                return IBufferFromPointer(rgbResp, sizeof(rgbResp));
            }
            else
            {
                return IBufferFromArray(R_APDU_GPO_MC);
            }
        }
        else
        {
            return IBufferFromArray(R_APDU_FAIL);
        }
        break;

    case CLAINS_READ_RECORD:
        DebugLog(L"-READ RECORD APDU received");
        if (bP1 != 0x01 || bP2 != 0x0c)
        {
            DebugLog(L"READ RECORD failed with unsupported options");
            return IBufferFromArray(R_APDU_FAIL);
        }

        {
            auto filename = L"ReadRecordResponse-" + m_paymentAidRegistration->Id.ToString() + L".dat";
            DebugLogString(filename);

            auto readFile = ReadAndUnprotectFileAsync(filename);
            readFile.wait();
            auto fileData = readFile.get();

            if (m_pbCurrentAppletId == AID_V)
            {
                // If there's no file, use some default data
                if (fileData == nullptr)
                {
                    DebugLog(L"No READ RECORD file found, using default data");
                    fileData = R_APDU_READ_RECORD_DEFAULT_V;
                }
                else
                {
                    DebugLog(L"File found for READ RECORD APDU response");
                }

                *pfComplete = true;
                return IBufferFromArray(fileData);
            }
            else if (m_pbCurrentAppletId == AID_MC)
            {
                // If there's no file, use some default data
                if (fileData == nullptr)
                {
                    DebugLog(L"No READ RECORD file found, using default data");
                    fileData = R_APDU_READ_RECORD_DEFAULT_MC;
                }
                else
                {
                    DebugLog(L"File found for READ RECORD APDU response");
                }

                return IBufferFromArray(fileData);
            }
            else
            {
                DebugLog(L"READ RECORD failed with wrong AID");
                return IBufferFromArray(R_APDU_FAIL);
            }
        }
        break;

    case CLAINS_COMPUTE_CRYPTO_CSUM:
        DebugLog(L"-CCC APDU received");

        if (m_pbCurrentAppletId != AID_MC)
        {
            DebugLog(L"CCC failed with unsupported options or mismatched AID");
            return IBufferFromArray(R_APDU_FAIL);
        }
        if (cbLE < R_APDU_COMPUTE_CRYPTO_CSUM_MC->Length)
        {
            BYTE rgbResp[] = { 0x6C, (BYTE)R_APDU_COMPUTE_CRYPTO_CSUM_MC->Length };
            return IBufferFromPointer(rgbResp, sizeof(rgbResp));
        }
        else
        {
            *pfComplete = true;
            return IBufferFromArray(R_APDU_COMPUTE_CRYPTO_CSUM_MC);
        }
        break;

    default:
        DebugLog(L"-Unknown APDU received");
        return IBufferFromArray(R_APDU_FAIL);
    }

    DebugLog(L"Fallthrough failure in ProcessCommandApdu");
    return IBufferFromArray(R_APDU_FAIL);
}

void BgTask::DebugLogString(String^ message)
{
    DebugLog(message->Data());
}

void BgTask::DebugLog(const wchar_t* pwstrMessage)
{
    auto strTime = GetCurrentTimeString();

    // To view output from OutputDebugString in the Output window of the debugger, go to the
    // Properties page of the Nfc project, then go to the Debug tab and select "Native Only" for
    // "Application process" in the "Debugger type" section
    OutputDebugStringW(pwstrMessage);
    OutputDebugStringW(L"\r\n");

    // Lock m_csDebugLog so that we can call this method outside the class lock m_csLock
    auto lock = m_csDebugLog.Lock();
    m_wsDebugLog += strTime->Data();
    m_wsDebugLog += L": ";
    m_wsDebugLog += pwstrMessage;
    m_wsDebugLog += L"\r\n";
}

void BgTask::FlushDebugLog()
{
    // Lock m_csDebugLog so that we can call this method outside the class lock m_csLock
    auto lock = m_csDebugLog.Lock();
    AppendFile(L"DebugLog.txt", ref new String(m_wsDebugLog.data()));
}
