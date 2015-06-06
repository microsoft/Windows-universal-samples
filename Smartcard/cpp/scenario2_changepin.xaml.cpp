// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario2_ChangePin.xaml.h"

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

Scenario2_ChangePin::Scenario2_ChangePin()
{
	InitializeComponent();
}

void Scenario2_ChangePin::ChangePin_Click(Platform::Object^ sender,
	Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (MainPage::Current->SmartCardReaderDeviceId == nullptr)
	{
		MainPage::Current->NotifyUser("Use Scenario One to create a TPM virtual smart card.", NotifyType::ErrorMessage);
		return;
	}

	Button^ b = dynamic_cast<Button^>(sender);
	b->IsEnabled = false;
	MainPage::Current->NotifyUser("Changing smart card PIN...", NotifyType::StatusMessage);

	create_task(MainPage::Current->GetSmartCard()).then(
		[=](task<Windows::Devices::SmartCards::SmartCard^> getCardTask)
	{
		Windows::Devices::SmartCards::SmartCard^ card = getCardTask.get();
		return SmartCardProvisioning::FromSmartCardAsync(card);
	}).then(
		[=](task<SmartCardProvisioning^> getProvisioningTask)
	{
		SmartCardProvisioning^ provisioning = getProvisioningTask.get();
		return provisioning->RequestPinChangeAsync();
	}).then(
		[=](task<bool> changePinTask)
	{
		try
		{
			bool result = changePinTask.get();
			if (result)
			{
				MainPage::Current->NotifyUser("Smart card change PIN operation completed.", NotifyType::StatusMessage);
			}
			else
			{
				MainPage::Current->NotifyUser("Smart card change PIN operation was canceled by the user.", NotifyType::StatusMessage);
			}
		}
		catch (COMException ^ex)
		{
			MainPage::Current->NotifyUser("Changing smart card PIN failed with exception: " + ex->ToString(), NotifyType::ErrorMessage);
		}

		b->IsEnabled = true;
	});
}