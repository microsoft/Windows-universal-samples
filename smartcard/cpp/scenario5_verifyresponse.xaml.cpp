// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario5_VerifyResponse.xaml.h"
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

Scenario5_VerifyResponse::Scenario5_VerifyResponse()
{
	InitializeComponent();
}

void Scenario5_VerifyResponse::VerifyResponse_Click(Platform::Object^ sender,
	Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (MainPage::Current->SmartCardReaderDeviceId == nullptr)
	{
		MainPage::Current->NotifyUser("Use Scenario One to create a TPM virtual smart card.", NotifyType::ErrorMessage);
		return;
	}

	Button^ b = dynamic_cast<Button^>(sender);
	b->IsEnabled = false;
	MainPage::Current->NotifyUser("Verifying smart card response...", NotifyType::StatusMessage);

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
		return context->VerifyResponseAsync(response);
	}).then(
		[=](task<bool> verifyResponseTask)
	{
		try
		{
			bool verifyResult = verifyResponseTask.get();
			MainPage::Current->NotifyUser("Smart card response verification completed. Result: " + verifyResult.ToString(), NotifyType::StatusMessage);
		}
		catch (COMException ^ex)
		{
			MainPage::Current->NotifyUser("Smart card response verification failed with exception: " + ex->ToString(), NotifyType::ErrorMessage);
		}

		b->IsEnabled = true;
	});
}