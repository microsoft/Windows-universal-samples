// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario4_ChangeAdminKey.xaml.h"
#include "ChallengeResponseAlgorithm.h"

using namespace Smartcard;
using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Devices::SmartCards;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::Security::Cryptography;
using namespace Windows::Security::Cryptography::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario4_ChangeAdminKey::Scenario4_ChangeAdminKey()
{
	InitializeComponent();
}

void Scenario4_ChangeAdminKey::ChangeAdminKey_Click(Platform::Object^ sender,
	Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (MainPage::Current->SmartCardReaderDeviceId == nullptr)
	{
		MainPage::Current->NotifyUser("Use Scenario One to create a TPM virtual smart card.", NotifyType::ErrorMessage);
		return;
	}

	Button^ b = dynamic_cast<Button^>(sender);
	b->IsEnabled = false;

	IBuffer^ newadminkey = CryptographicBuffer::GenerateRandom(ADMIN_KEY_LENGTH_IN_BYTES);

	MainPage::Current->NotifyUser("Changing smart card admin key...", NotifyType::StatusMessage);

	create_task(MainPage::Current->GetSmartCard()).then(
		[=](task<Windows::Devices::SmartCards::SmartCard^> getSmartCardTask)
	{
		Windows::Devices::SmartCards::SmartCard^ card = getSmartCardTask.get();
		return SmartCardProvisioning::FromSmartCardAsync(card);
	}).then(
		[=](task<SmartCardProvisioning^> getProvisioningTask)
	{
		SmartCardProvisioning^ provisioning = getProvisioningTask.get();
		return provisioning->GetChallengeContextAsync();
	}).then(
		[=](task<SmartCardChallengeContext^> getChallengeContextTask)
	{
		SmartCardChallengeContext^ context = getChallengeContextTask.get();
		IBuffer^ response = ChallengeResponseAlgorithm::CalculateResponse(context->Challenge, MainPage::Current->AdminKey);

		return context->ChangeAdministrativeKeyAsync(response, newadminkey);
	}).then(
		[=](task<void> changeAdminKeyTask)
	{
		try
		{
			changeAdminKeyTask.get();
			MainPage::Current->AdminKey = newadminkey;
			MainPage::Current->NotifyUser("Smart card change admin key operation completed.", NotifyType::StatusMessage);
		}
		catch (COMException ^ex)
		{
			MainPage::Current->NotifyUser("Changing smart card admin key failed with exception." + ex->ToString(), NotifyType::ErrorMessage);
		}

		b->IsEnabled = true;
	});

}