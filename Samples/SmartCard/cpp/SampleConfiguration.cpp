// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;
using namespace Concurrency;
using namespace Windows::Devices::SmartCards;
using namespace Windows::Foundation::Collections;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Create and provision a TPM virtual smart card", "SDKTemplate.Scenario1_CreateTPMVSC" },
    { "Change smart card PIN", "SDKTemplate.Scenario2_ChangePin" },
    { "Reset smart card PIN", "SDKTemplate.Scenario3_ResetPin" },
    { "Change smart card admin key", "SDKTemplate.Scenario4_ChangeAdminKey" },
    { "Verify response", "SDKTemplate.Scenario5_VerifyResponse" },
    { "Delete TPM virtual smart card", "SDKTemplate.Scenario6_DeleteTPMVSC" },
    { "List all smart cards", "SDKTemplate.Scenario7_ListSmartCards" },
    { "Transmit APDU to smart card", "SDKTemplate.Scenario8_Transmit" }
};

task<Windows::Devices::SmartCards::SmartCard^> MainPage::GetSmartCard()
{
    return create_task(SmartCardReader::FromIdAsync(deviceId)).then(
        [&](task<SmartCardReader^> getReaderTask)
    {
        SmartCardReader^ reader = getReaderTask.get();
        return reader->FindAllCardsAsync();
    }).then(
        [&](task<IVectorView<SmartCard^>^>
        getCardsTask)
    {
        IVectorView<SmartCard^>^ cards = getCardsTask.get();
        return cards->GetAt(0);
    });
}