// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario8_Transmit.xaml.h"

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

Scenario8_Transmit::Scenario8_Transmit()
{
	InitializeComponent();
}

void Scenario8_Transmit::Transmit_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (MainPage::Current->SmartCardReaderDeviceId == nullptr)
	{
		MainPage::Current->NotifyUser("Use Scenario One to create a TPM virtual smart card.", NotifyType::ErrorMessage);
		return;
	}

	Button^ b = dynamic_cast<Button^>(sender);
	b->IsEnabled = false;

	MainPage::Current->NotifyUser("Transmitting APDU...", NotifyType::StatusMessage);

	// Read EF.ATR file
	// The command is meant specifically for GIDS cards (such as TPM VSCs), 
	// and will fail on other types.
	Platform::Array<unsigned char>^ readEfAtrBytes = { 0x00, 0xCB, 0x2F, 0x01, 0x02, 0x5C, 0x00, 0xFF };

	create_task(MainPage::Current->GetSmartCard()).then(
		[=](task<Windows::Devices::SmartCards::SmartCard^> getCardTask)
	{
		Windows::Devices::SmartCards::SmartCard^ card = getCardTask.get();
		return card->ConnectAsync();
	}).then(
		[=](task<SmartCardConnection^> getConnectionTask)
	{
		SmartCardConnection^ connection = getConnectionTask.get();

		IBuffer^ readEfAtr = CryptographicBuffer::CreateFromByteArray(readEfAtrBytes);

		return connection->TransmitAsync(readEfAtr);
	}).then(
		[=](task<IBuffer^> getResultTask)
	{
		try
		{
			IBuffer^ result = getResultTask.get();

			MainPage::Current->NotifyUser("Response received: " + CryptographicBuffer::EncodeToHexString(result), NotifyType::StatusMessage);
		}
		catch (COMException ^ex)
		{
			MainPage::Current->NotifyUser("Transmit APDU failed with exception: " + ex->ToString(), NotifyType::ErrorMessage);
		}

		b->IsEnabled = true;
	});
}