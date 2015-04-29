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
	{ "Create and provision a TPM virtual smart card", "Smartcard.Scenario1_CreateTPMVSC" },
	{ "Change smart card PIN", "Smartcard.Scenario2_ChangePin" },
	{ "Reset smart card PIN", "Smartcard.Scenario3_ResetPin" },
	{ "Change smart card admin key", "Smartcard.Scenario4_ChangeAdminKey" },
	{ "Verify response", "Smartcard.Scenario5_VerifyResponse" },
	{ "Delete TPM virtual smart card", "Smartcard.Scenario6_DeleteTPMVSC" },
	{ "List all smart cards", "Smartcard.Scenario7_ListSmartCards" },
	{ "Transmit APDU to smart card", "Smartcard.Scenario8_Transmit" }
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