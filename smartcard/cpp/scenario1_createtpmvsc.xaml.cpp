// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_CreateTPMVSC.xaml.h"
#include "ChallengeResponseAlgorithm.h"

using namespace Smartcard;
using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Devices::SmartCards;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::NumberFormatting;
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

Scenario1_CreateTPMVSC::Scenario1_CreateTPMVSC() : reader(nullptr)
{
	InitializeComponent();
}

void Scenario1_CreateTPMVSC::Create_Click(Platform::Object^ sender,
	Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Button^ b = dynamic_cast<Button^>(sender);
	b->IsEnabled = false;

	SmartCardPinPolicy^ pinPolicy = nullptr;
	try
	{
		pinPolicy = ParsePinPolicy();
	}
	catch (Exception^ ex)
	{
		MainPage::Current->NotifyUser("Failed to parse PIN policy due to exception: " + ex->ToString(), NotifyType::ErrorMessage);
		b->IsEnabled = true;
		return;
	}

	MainPage::Current->AdminKey = CryptographicBuffer::GenerateRandom(ADMIN_KEY_LENGTH_IN_BYTES);

	MainPage::Current->NotifyUser("Creating TPM virtual smart card...", NotifyType::StatusMessage);

	create_task(SmartCardProvisioning::RequestVirtualSmartCardCreationAsync(FriendlyName->Text, MainPage::Current->AdminKey, pinPolicy)).then(
		[=](task<SmartCardProvisioning^> createCardTask)
	{
		SmartCardProvisioning^ provisioning = createCardTask.get();

		// If the user selects Cancel on the card creation prompt,
		// nullptr will be returned.  In this case, we need to cancel
		// the following asynchronous calls.
		if (nullptr == provisioning)
		{
			cancel_current_task();
		}

		// The following block is not directly related to TPM virtual smart
		// card creation, but rather demonstrates how to handle CardAdded
		// events by registering an event handler with a SmartCardReader
		// object.  Since we are using a TPM virtual smart card in this
		// case, the card cannot actually be added to or removed from
		// the reader, but a CardAdded event will fire as soon as the
		// handler is added.
		{
			// First, get the SmartCardReader for the reader we want to
			// receive events for.  It is important to note that the
			// event handler will only be fired during the lifetime of
			// this particular SmartCardReader instance; i.e., in this
			// case, if this->reader is destructed, events will no
			// longer be fired.  Thus, we must maintain a handle
			// to it as an instance member so that its reference
			// count remains non-zero for the lifetime of the page.
			this->reader = provisioning->SmartCard->Reader;

			// Here we use the += operator to add our event handler to the
			// collection of CardAdded handlers for the reader.  Note that
			// if you want to do any UI operations in the handler, or in
			// general use any non-agile objects, you must specify a
			// callback context of CallbackContext::Same in the
			// TypedEventHandler's constructor, as below.  Otherwise you
			// may receive exceptions due to RPC_E_WRONG_THREAD.
			this->reader->CardAdded += ref new
				TypedEventHandler<SmartCardReader ^, CardAddedEventArgs ^>(this, &Scenario1_CreateTPMVSC::HandleCardAdded, Platform::CallbackContext::Same);
		}

		MainPage::Current->SmartCardReaderDeviceId = provisioning->SmartCard->Reader->DeviceId;

		// We now have a TPM virtual smart card which is provisioned and
		// ready to use; therefore, the following steps are not strictly
		// necessary.  However, should you want to re-provision the card
		// in the future, you would first get a challenge context from
		// the provisioning object and use that to re-provision the card.
		return provisioning->GetChallengeContextAsync();
	}).then(
		[=](task<SmartCardChallengeContext^> getChallengeTask)
	{
		SmartCardChallengeContext^ context = getChallengeTask.get();

		IBuffer^ response = ChallengeResponseAlgorithm::CalculateResponse(context->Challenge, MainPage::Current->AdminKey);

		return context->ProvisionAsync(response, true);
	}).then(
		[=](task<void> provisionTask)
	{
		try
		{
			provisionTask.get();
			MainPage::Current->NotifyUser("TPM virtual smart card is provisioned and ready for use.", NotifyType::StatusMessage);
		}
		catch (COMException ^ex)
		{
			// Two potentially common error scenarios when creating a TPM
			// virtual smart card are that the user's machine may not have
			// a TPM, or the TPM may not be ready for use.  It is important
			// to explicitly check for these scenarios by checking the
			// HResult of any exceptions thrown by
			// RequestVirtualSmartCardCreationAsync and gracefully
			// providing a suitable message to the user.
			if (NTE_DEVICE_NOT_FOUND == ex->HResult)
			{
				MainPage::Current->NotifyUser("We were unable to find a Trusted Platform Module on your machine. A TPM is required to use a TPM Virtual Smart Card.", NotifyType::ErrorMessage);
			}
			else if (NTE_DEVICE_NOT_READY == ex->HResult)
			{
				MainPage::Current->NotifyUser("Your Trusted Platform Module is not ready for use. Please contact your administrator for assistance with initializing your TPM.", NotifyType::ErrorMessage);
			}
			else
			{
				MainPage::Current->NotifyUser("TPM virtual smart card creation failed with exception: " + ex->ToString(), NotifyType::ErrorMessage);
			}
		}
		catch (const task_canceled&)
		{
			// If the user cancelled the card creation prompt and
			// cancel_current_task was called above, subsequent blocks in the
			// async chain will throw task_canceled, which will be caught here.
			MainPage::Current->NotifyUser("TPM virtual smart card creation was canceled by the user.", NotifyType::StatusMessage);
		}

		b->IsEnabled = true;
	});
}

void Scenario1_CreateTPMVSC::HandleCardAdded(SmartCardReader^ evtReader, CardAddedEventArgs^ args)
{
	MainPage::Current->NotifyUser("Card added to reader " + evtReader->Name + ".", NotifyType::StatusMessage);
}

SmartCardPinPolicy^ Scenario1_CreateTPMVSC::ParsePinPolicy()
{
	SmartCardPinPolicy^ pinPolicy = ref new SmartCardPinPolicy();

	DecimalFormatter^ formatter = ref new DecimalFormatter();
	Platform::IBox<unsigned long long>^ minLength = formatter->ParseUInt(PinMinLength->Text);
	Platform::IBox<unsigned long long>^ maxLength = formatter->ParseUInt(PinMaxLength->Text);

	// Verify that the value parsed by the DecimalFormatter is valid.
	// If the text box is empty or contains a non-integer, the IBox
	// will be nullptr.
	if (nullptr == minLength || nullptr == maxLength)
	{
		throw ref new Exception(NTE_INVALID_PARAMETER);
	}

	// Note: the following cast is unsafe, but in this situation
	// it is not a problem as the maximum allowable MinLength
	// and MaxLength values are much smaller than MAXUINT.
	pinPolicy->MinLength = (unsigned int)minLength->Value;
	pinPolicy->MaxLength = (unsigned int)maxLength->Value;

	switch (PinUppercase->SelectedIndex)
	{
	case pinPolicyDisallowedIndex:
		pinPolicy->UppercaseLetters = SmartCardPinCharacterPolicyOption::Disallow;
		break;
	case pinPolicyAllowedIndex:
		pinPolicy->UppercaseLetters = SmartCardPinCharacterPolicyOption::Allow;
		break;
	case pinPolicyRequireOneIndex:
		pinPolicy->UppercaseLetters = SmartCardPinCharacterPolicyOption::RequireAtLeastOne;
		break;
	default:
		throw ref new Exception(NTE_INVALID_PARAMETER);
	}

	switch (PinLowercase->SelectedIndex)
	{
	case pinPolicyDisallowedIndex:
		pinPolicy->LowercaseLetters = SmartCardPinCharacterPolicyOption::Disallow;
		break;
	case pinPolicyAllowedIndex:
		pinPolicy->LowercaseLetters = SmartCardPinCharacterPolicyOption::Allow;
		break;
	case pinPolicyRequireOneIndex:
		pinPolicy->LowercaseLetters = SmartCardPinCharacterPolicyOption::RequireAtLeastOne;
		break;
	default:
		throw ref new Exception(NTE_INVALID_PARAMETER);
	}

	switch (PinDigits->SelectedIndex)
	{
	case pinPolicyDisallowedIndex:
		pinPolicy->Digits = SmartCardPinCharacterPolicyOption::Disallow;
		break;
	case pinPolicyAllowedIndex:
		pinPolicy->Digits = SmartCardPinCharacterPolicyOption::Allow;
		break;
	case pinPolicyRequireOneIndex:
		pinPolicy->Digits = SmartCardPinCharacterPolicyOption::RequireAtLeastOne;
		break;
	default:
		throw ref new Exception(NTE_INVALID_PARAMETER);
	}

	switch (PinSpecial->SelectedIndex)
	{
	case pinPolicyDisallowedIndex:
		pinPolicy->SpecialCharacters = SmartCardPinCharacterPolicyOption::Disallow;
		break;
	case pinPolicyAllowedIndex:
		pinPolicy->SpecialCharacters = SmartCardPinCharacterPolicyOption::Allow;
		break;
	case pinPolicyRequireOneIndex:
		pinPolicy->SpecialCharacters = SmartCardPinCharacterPolicyOption::RequireAtLeastOne;
		break;
	default:
		throw ref new Exception(NTE_INVALID_PARAMETER);
	}

	return pinPolicy;
}