// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario7_ListSmartCards.xaml.h"

using namespace Smartcard;
using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Devices::Enumeration;
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

Scenario7_ListSmartCards::Scenario7_ListSmartCards()
{
	InitializeComponent();
}

SmartCardItem::SmartCardItem() : readerName(""), cardName("")
{

}

void Scenario7_ListSmartCards::ListSmartCards_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Button^ b = dynamic_cast<Button^>(sender);
	b->IsEnabled = false;

	// Create a Vector of managed pointers to SmartCardItems to display in our
	// list.  We can immediately assign ItemListView's source to be this new
	// Vector, as the ItemListView will subscribe to the Vector's VectorChanged
	// event and update itself when the vector changes.  This turns out to be
	// very convenient given the asynchronous operations we must use to list the
	// cards.
	smartCardItems = ref new Platform::Collections::Vector<SmartCardItem^>();
	ItemListView->ItemsSource = smartCardItems;

	MainPage::Current->NotifyUser("Enumerating smart cards...", NotifyType::StatusMessage);

	String^ selector = SmartCardReader::GetDeviceSelector();
	create_task(DeviceInformation::FindAllAsync(selector)).then(
		[&](task<DeviceInformationCollection^> getDevicesTask)
	{
		// From DeviceInformationCollection::FindAllAsync, we start with a
		// DeviceInformationCollection which is essentially a vector of
		// DeviceInformation objects.  For each of these objects, we perform
		// the following steps.
		DeviceInformationCollection^ devices = getDevicesTask.get();
		for (UINT32 deviceIndex = 0; deviceIndex < devices->Size; ++deviceIndex)
		{
			// 1. Instantiate a SmartCardReader using 
			//    SmartCardReader::FromIdAsync
			create_task(SmartCardReader::FromIdAsync(devices->GetAt(deviceIndex)->Id)).then(
				[](task<SmartCardReader^> getReaderTask)
			{
				// 2. Enumerate all the cards attached to that reader using
				//    SmartCardReader::FindAllCardsAsync
				SmartCardReader^ reader = getReaderTask.get();
				return reader->FindAllCardsAsync();
			}).then(
				[this](task<IVectorView<Windows::Devices::SmartCards::SmartCard^>^> getCardsTask)
			{
				std::vector<task<SmartCardProvisioning^>> provisioningTasks;

				IVectorView<Windows::Devices::SmartCards::SmartCard^>^ cards = getCardsTask.get();
				for (UINT32 cardIndex = 0; cardIndex < cards->Size; ++cardIndex)
				{
					Windows::Devices::SmartCards::SmartCard^ card = cards->GetAt(cardIndex);

					// 3. For each card, instantiate a SmartCardProvisioning
					//    object using SmartCardProvisioning::FromSmartCardAsync
					provisioningTasks.push_back(create_task(SmartCardProvisioning::FromSmartCardAsync(card)));
				}

				when_all(begin(provisioningTasks), end(provisioningTasks)).then(
					[=](std::vector<SmartCardProvisioning^> provisioningVector)
				{
					for (std::vector<SmartCardProvisioning^>::iterator it = provisioningVector.begin(); it < provisioningVector.end(); ++it)
					{
						SmartCardProvisioning^ provisioning = *it;

						// 4. Once we have a SmartCardProvisioningObject for
						//    each card, we need to use it to get the name of
						//    the card.  This is another asynchronous operation.
						create_task(provisioning->GetNameAsync()).then(
							[=](task<String^> getNameTask)
						{
							String^ cardName = "";
							try
							{
								cardName = getNameTask.get();
							}
							catch (COMException^ ex)
							{
								cardName = "Exception: " + ex->ToString();
							}

							// 5. Finally, now that we have the card name and
							//    a provisioning object which contains a chain
							//    of references we can use to get the reader
							//    name, we can create a new SmartCardItem and
							//    add it to our list.  Recall that the list view
							//    which is the graphical representation of this
							//    list will be updated automatically.
							SmartCardItem^ item = ref new SmartCardItem;
							item->ReaderName = provisioning->SmartCard->Reader->Name;
							item->CardName = cardName;
							smartCardItems->InsertAt(0, item);
						});
					}
				}).then(
					[](task<void> checkExceptionTask)
				{
					// This then(...) lambda follows the when_all(...) for
					// provisioningTasks above and will run once all of the
					// SmartCardProvisioning::FromSmartCardAsync have completed.
					// Note that because we're still waiting on
					// SmartCardProvisioning::GetNameAsync calls, the method
					// could still fail after this then executes.  However,
					// if that happens we will use the actual card card item
					// to indicate the exception.  Ideally we would do a
					// when_all for the tasks which get the reader names, but
					// we need the lambdas for those tasks to have access to
					// their corresponding provisioning objects so we can still
					// get the reader name, which makes them ill-suited to
					// when_all.
					try
					{
						checkExceptionTask.get();
						MainPage::Current->NotifyUser("Enumerating smart cards completed.", NotifyType::StatusMessage);
					}
					catch (COMException^ ex)
					{
						MainPage::Current->NotifyUser("Enumerating smart cards failed with exception: " + ex->ToString(), NotifyType::ErrorMessage);
					}
				});
			}).then(
				[](task<void> checkExceptionTask)
			{
				try
				{
					checkExceptionTask.get();
				}
				catch (COMException^ ex)
				{
					MainPage::Current->NotifyUser("Enumerating smart cards failed with exception: " + ex->ToString(), NotifyType::ErrorMessage);
				}
			});
		}
	}).then(
		[](task<void> checkExceptionTask)
	{
		try
		{
			checkExceptionTask.get();
		}
		catch (COMException^ ex)
		{
			MainPage::Current->NotifyUser("Enumerating smart cards failed with exception: " + ex->ToString(), NotifyType::ErrorMessage);
		}
	});

	b->IsEnabled = true;
}