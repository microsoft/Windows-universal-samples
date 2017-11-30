// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario7_ListSmartCards.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::SmartCards;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario7_ListSmartCards::Scenario7_ListSmartCards()
{
    InitializeComponent();
}

void Scenario7_ListSmartCards::ListSmartCards_Click(Object^ sender, RoutedEventArgs^ e)
{
    Button^ b = safe_cast<Button^>(sender);
    b->IsEnabled = false;

    // Create a Vector of managed pointers to SmartCardItems to display in our
    // list.  We can immediately assign ItemListView's source to be this new
    // Vector, as the ItemListView will subscribe to the Vector's VectorChanged
    // event and update itself when the vector changes.  This turns out to be
    // very convenient given the asynchronous operations we must use to list the
    // cards.
    smartCardItems = ref new Vector<SmartCardItem^>();
    ItemListView->ItemsSource = smartCardItems;

    MainPage::Current->NotifyUser("Enumerating smart cards...", NotifyType::StatusMessage);

    String^ selector = SmartCardReader::GetDeviceSelector();
    create_task(DeviceInformation::FindAllAsync(selector)).then(
        [this](DeviceInformationCollection^ devices)
    {
        std::vector<task<void>> deviceTasks;

        // From DeviceInformationCollection::FindAllAsync, we start with a
        // DeviceInformationCollection which is essentially a vector of
        // DeviceInformation objects.  For each of these objects, we perform
        // the following steps.
        for (DeviceInformation^ info : devices)
        {
            // 1. Instantiate a SmartCardReader using 
            //    SmartCardReader::FromIdAsync
            task<void> deviceTask = create_task(SmartCardReader::FromIdAsync(info->Id)).then(
                [](SmartCardReader^ reader)
            {
                // 2. Enumerate all the cards attached to that reader using
                //    SmartCardReader::FindAllCardsAsync
                return reader->FindAllCardsAsync();
            }).then(
                [this](IVectorView<SmartCard^>^ cards)
            {
                std::vector<task<void>> getNameTasks;

                for (SmartCard^ card : cards)
                {
                    // 3. For each card, instantiate a SmartCardProvisioning
                    //    object using SmartCardProvisioning::FromSmartCardAsync
                    task<void> getNameTask = create_task(SmartCardProvisioning::FromSmartCardAsync(card)).then(
                        [this](SmartCardProvisioning^ provisioning)
                    {
                        // 4. Once we have a SmartCardProvisioningObject for
                        //    each card, we need to use it to get the name of
                        //    the card.  This is another asynchronous operation.
                        return create_task(provisioning->GetNameAsync()).then(
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
                    });
                    getNameTasks.push_back(getNameTask);
                }

                return when_all(begin(getNameTasks), end(getNameTasks));
            });
            deviceTasks.push_back(deviceTask);
        }
        return when_all(begin(deviceTasks), end(deviceTasks));
    }).then(
        [b](task<void> checkExceptionTask)
    {
        try
        {
            checkExceptionTask.get();
            MainPage::Current->NotifyUser("Enumerating smart cards completed.", NotifyType::StatusMessage);
        }
        catch (COMException^ ex)
        {
            MainPage::Current->NotifyUser("Enumerating smart cards failed with exception: " + ex->ToString(), NotifyType::ErrorMessage);
        }
        b->IsEnabled = true;
    });
}