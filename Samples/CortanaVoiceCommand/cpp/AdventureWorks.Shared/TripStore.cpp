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
#include "TripStore.h"

using namespace AdventureWorks_Shared;

using namespace Microsoft::WRL;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;

TripStore::TripStore()
{
    loaded = false;
    this->trips = ref new Vector<Trip^>();
}

/// <summary> Asynchronously load trips </summary>
void TripStore::LoadTrips()
{
    // load asynchronously, without waiting.
    LoadTripsTask();
}

/// <summary> Load trips synchronously. Used by the background task service. </summary>
void TripStore::LoadTripsSync()
{
    LoadTripsTask().wait();
}

/// <summary> Read the xml data from the provided stream and extract trip data </summary>
/// <param name="randomAccessReadStream">The stream pointing to trips.xml</param>
HRESULT TripStore::ReadXml(IRandomAccessStream^ randomAccessReadStream)
{
    HRESULT hr;
    ComPtr<IStream> readStream;
    ComPtr<IXmlReader> reader;
    XmlNodeType nodeType;

    Trip^ trip = nullptr;
    String^ currField = nullptr;

    ChkHr(::CreateStreamOverRandomAccessStream(randomAccessReadStream, IID_PPV_ARGS(&readStream)));
    ChkHr(::CreateXmlReader(IID_PPV_ARGS(&reader), nullptr));
    ChkHr(reader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit));
    ChkHr(reader->SetInput(readStream.Get()));

    // process into Trip objects.
    while (S_OK == (hr = reader->Read(&nodeType)))
    {
        PCWSTR localName = nullptr;

        unsigned int localNameSize = 0;

        switch (nodeType)
        {
        case XmlNodeType_Element:
            ChkHr(reader->GetLocalName(&localName, &localNameSize));
            if (localNameSize > 0 && localName != nullptr)
            {
                if (wcsncmp(L"Trip", localName, localNameSize) == 0)
                {
                    // before creating a new trip, append the current one to the trip set.
                    if (trip != nullptr)
                    {
                        this->Trips->Append(trip);
                    }

                    trip = ref new Trip();
                    currField = nullptr;
                }
                else if (trip != nullptr)
                {
                    if (wcsncmp(L"Destination", localName, localNameSize) == 0)
                    {
                        currField = ref new String(localName);
                    }
                    else if (wcsncmp(L"Description", localName, localNameSize) == 0)
                    {
                        currField = ref new String(localName);
                    }
                    else if (wcsncmp(L"StartDate", localName, localNameSize) == 0)
                    {
                        currField = ref new String(localName);
                    }
                    else if (wcsncmp(L"EndDate", localName, localNameSize) == 0)
                    {
                        currField = ref new String(localName);
                    }
                    else if (wcsncmp(L"Notes", localName, localNameSize) == 0)
                    {
                        currField = ref new String(localName);
                    }
                    else
                    {
                        if (trip != nullptr)
                        {
                            this->Trips->Append(trip);
                        }

                        trip = nullptr;
                        currField = nullptr;
                    }
                }
            }
            break;
        case XmlNodeType_Text:
            if (trip == nullptr)
            {
                break;
            }

            PCWSTR textFieldValue;
            unsigned int textFieldSize;
            ChkHr(reader->GetValue(&textFieldValue, &textFieldSize));
            if (currField == L"Destination")
            {
                trip->Destination = ref new String(textFieldValue);
            }
            else if (currField == L"Description")
            {
                trip->Description = ref new String(textFieldValue);
            }
            else if (currField == L"StartDate")
            {
                trip->StartDate = ref new Calendar();
                std::tm datetime;
                std::wistringstream ss(textFieldValue);

                // 
                ss >> std::get_time(&datetime, L"%Y-%m-%d");
                if (ss.fail())
                {
                    OutputDebugStringW(L"Failed to parse start date?");
                    ChkHr(E_INVALIDARG);
                }
                else
                {
                    trip->StartDate->Day = datetime.tm_mday;
                    trip->StartDate->Year = datetime.tm_year + 1900;
                    trip->StartDate->Month = datetime.tm_mon + 1;
                }
            }
            else if (currField == L"EndDate")
            {
                trip->EndDate = ref new Calendar();
                std::tm datetime;
                std::wistringstream ss(textFieldValue);

                // 
                ss >> std::get_time(&datetime, L"%Y-%m-%d");
                if (ss.fail())
                {
                    OutputDebugStringW(L"Failed to parse end date?");
                    ChkHr(E_INVALIDARG);
                }
                else
                {
                    trip->EndDate->Day = datetime.tm_mday;
                    trip->EndDate->Year = datetime.tm_year + 1900;
                    trip->EndDate->Month = datetime.tm_mon + 1;
                }
            }
            else if (currField == L"Notes")
            {
                trip->Notes = ref new String(textFieldValue);
            }

            break;
        default:
            break;
        }
    }

    // handle the final trip added.
    if (trip != nullptr)
    {
        this->Trips->Append(trip);
    }

    return hr;
}

/// <summary>Initiate a task to begin loading trip data, or initialize starting
/// data if the trip data file does not exist yet.</summary>
/// <returns>Returns a task that will complete when the trips are loaded.</returns>
Concurrency::task<void> AdventureWorks_Shared::TripStore::LoadTripsTask()
{
    if (loaded)
    {
        return create_task([]() {});
    }

    StorageFolder^ folder = ApplicationData::Current->LocalFolder;
    this->Trips->Clear();

    return create_task(folder->TryGetItemAsync(L"trips.xml"), task_continuation_context::use_current())
        .then([this](task<IStorageItem^> storageItemTask)
    {
        IStorageItem^ item = storageItemTask.get();
        if (item == nullptr)
        {
            Trip^ trip = ref new Trip();
            trip->Destination = L"London";
            trip->Description = L"Trip to London!";
            trip->StartDate = ref new Calendar();
            trip->StartDate->Month = 5;
            trip->StartDate->Day = 5;
            trip->StartDate->Year = 2015;

            trip->EndDate = ref new Calendar();
            trip->EndDate->Month = 5;
            trip->EndDate->Day = 15;
            trip->EndDate->Year = 2015;

            this->Trips->Append(trip);

            trip = ref new Trip();
            trip->Destination = L"Melbourne";
            trip->Description = L"Trip to Australia";
            trip->StartDate = ref new Calendar();
            trip->StartDate->Month = 2;
            trip->StartDate->Day = 2;
            trip->StartDate->Year = 2016;

            trip->EndDate = ref new Calendar();

            trip->EndDate->Month = 5;
            trip->EndDate->Day = 17;
            trip->EndDate->Year = 2016;
            trip->Notes = L"Bring Sunscreen!";

            this->Trips->Append(trip);

            trip = ref new Trip();
            trip->Destination = L"Yosemite National Park";
            trip->Description = L"Trip to Yosemite!";
            trip->StartDate = ref new Calendar();
            trip->StartDate->Month = 11;
            trip->StartDate->Day = 7;
            trip->StartDate->Year = 2015;

            trip->EndDate = ref new Calendar();
            trip->EndDate->Month = 7;
            trip->EndDate->Day = 19;
            trip->EndDate->Year = 2015;
            trip->Notes = L"Buy some new hiking boots";

            this->Trips->Append(trip);

            return create_task([this]()
            {
                this->WriteTrips();
            });
        }
        else if (item->IsOfType(StorageItemTypes::File))
        {
            StorageFile^ tripsFile = dynamic_cast<StorageFile^>(item);
            if (tripsFile != nullptr)
            {
                return create_task(tripsFile->OpenAsync(FileAccessMode::Read), task_continuation_context::use_current()).then([this](IRandomAccessStream^ readStream)
                {
                    HRESULT hr = ReadXml(readStream);
                    if (FAILED(hr))
                    {
                        OutputDebugStringW(L"Failed reading trips.xml");
                    }
                    this->loaded = true;
                });
            }
        }
        return create_task([]() {});
    });

}

IObservableVector<Trip^>^ TripStore::Trips::get()
{
    return this->trips;
}

/// <summary> Delete the specified trip, if it exists in the store, and save
/// the trip store </summary>
/// <param name="trip">the trip to delete</param>
IAsyncAction ^ TripStore::DeleteTripAsync(Trip ^ trip)
{
    OutputDebugStringW(L"TODO Delete trip from store");
    for (unsigned int i = 0; i < this->Trips->Size; i++)
    {
        if (this->Trips->GetAt(i) == trip)
        {
            this->Trips->RemoveAt(i);
            break;
        }
    }

    return create_async([this, trip]()
    {
        this->WriteTrips();
    });
}

/// <summary>Add the specified trip to the store, if it doesn't exist, and save
/// the trip store </summary>
/// <param name="trip">the trip to add</param>
IAsyncAction ^ TripStore::SaveTripAsync(Trip ^ trip)
{
    return create_async([this, trip]()
    {
        unsigned int index = 0;
        if (!this->Trips->IndexOf(trip, &index))
        {
            /// note, currently this can't be called from the background thread.
            CoreApplicationView^ view = CoreApplication::MainView;
            view->CoreWindow->Dispatcher->RunAsync(
                CoreDispatcherPriority::Normal,
                ref new DispatchedHandler([this, trip]()
            {
                this->Trips->Append(trip);
            }));
        }

        this->WriteTrips();
    });

}

/// <summary> Return a task that loads the trip store, which can be waited on</summary>
/// <returns> IAsyncAction task that completes once the data is loaded</returns>
Windows::Foundation::IAsyncAction ^ AdventureWorks_Shared::TripStore::LoadTripsAsync()
{
    return create_async(
        [this]() {
        return LoadTripsTask();
    });
}

/// <summary> Trigger asynchronous writing of trip data to disk</summary>
void TripStore::WriteTrips()
{
    StorageFolder^ folder = Windows::Storage::ApplicationData::Current->LocalFolder;

    create_task(folder->CreateFileAsync(L"trips.xml", CreationCollisionOption::ReplaceExisting)).then([this](StorageFile^ file)
    {
        create_task(file->OpenAsync(FileAccessMode::ReadWrite)).then([file, this](IRandomAccessStream^ writeStream)
        {
            HRESULT hr = S_OK;

            ComPtr<IStream> tripsOutputStream;
            ComPtr<IXmlWriter> tripsXmlWriter;
            auto formatter = ref new DateTimeFormatter("{year.full}-{month.integer(2)}-{day.integer(2)}");

            ChkHr(::CreateStreamOverRandomAccessStream(writeStream, IID_PPV_ARGS(&tripsOutputStream)));
            ChkHr(::CreateXmlWriter(IID_PPV_ARGS(&tripsXmlWriter), nullptr));
            ChkHr(tripsXmlWriter->SetOutput(tripsOutputStream.Get()));
            ChkHr(tripsXmlWriter->SetProperty(XmlWriterProperty_Indent, TRUE));

            ChkHr(tripsXmlWriter->WriteStartDocument(XmlStandalone_Omit));
            ChkHr(tripsXmlWriter->WriteStartElement(nullptr, L"Root", nullptr));

            for (unsigned int i = 0; i < this->Trips->Size; i++)
            {
                Trip^ trip = this->Trips->GetAt(i);

                ChkHr(tripsXmlWriter->WriteStartElement(nullptr, L"Trip", nullptr));


                ChkHr(tripsXmlWriter->WriteStartElement(nullptr, L"Destination", nullptr));
                ChkHr(tripsXmlWriter->WriteString(trip->Destination->Data()));
                ChkHr(tripsXmlWriter->WriteEndElement());

                // Description, may be null/empty
                ChkHr(tripsXmlWriter->WriteStartElement(nullptr, L"Description", nullptr));
                if ((trip->Description != nullptr) && !trip->Description->IsEmpty())
                {
                    ChkHr(tripsXmlWriter->WriteString(trip->Description->Data()));
                }
                ChkHr(tripsXmlWriter->WriteEndElement());

                // StartDate field, may be null, format using expected xml date format. (yyyy-mm-dd)
                ChkHr(tripsXmlWriter->WriteStartElement(nullptr, L"StartDate", nullptr));
                if (trip->StartDate != nullptr)
                {
                    String^ str = formatter->Format(trip->StartDate->GetDateTime());
                    ChkHr(tripsXmlWriter->WriteString(str->Data()));
                }
                ChkHr(tripsXmlWriter->WriteEndElement());

                // EndDate field, may be null, format using expected xml date format. (yyyy-mm-dd)
                ChkHr(tripsXmlWriter->WriteStartElement(nullptr, L"EndDate", nullptr));
                if (trip->EndDate != nullptr)
                {
                    String^ str = formatter->Format(trip->EndDate->GetDateTime());
                    ChkHr(tripsXmlWriter->WriteString(str->Data()));
                }
                ChkHr(tripsXmlWriter->WriteEndElement());

                // Notes, may be null
                ChkHr(tripsXmlWriter->WriteStartElement(nullptr, L"Notes", nullptr));
                if ((trip->Notes != nullptr) && !trip->Notes->IsEmpty())
                {
                    ChkHr(tripsXmlWriter->WriteString(trip->Notes->Data()));
                }
                ChkHr(tripsXmlWriter->WriteEndElement());

                // end Trip tag.
                ChkHr(tripsXmlWriter->WriteEndElement());
            };

            return hr;
        });
    });
}
