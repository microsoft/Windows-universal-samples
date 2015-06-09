#pragma once

#include "pch.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    inline void InitializeRevisionNumber()
    {
        // This initializes the value used throughout the sample which tracks the expected index revision number. This
        // value is used to check if the app's expected revision number matches the actual index revision number and is
        // stored in the app's local settings to allow it to persist across termination.
        auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
        if (!localSettings->Values->HasKey(L"expectedIndexRevision"))
        {
            localSettings->Values->Insert(L"expectedIndexRevision", ref new Platform::Box<unsigned long long>(0));
        }
    }

    inline void OnIndexerOperationBegin()
    {
        // Update the expected index revision number in expectation of the call that's about to be made
        auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
        Platform::IBox<unsigned long long>^ revisionNumber = dynamic_cast<Platform::IBox<unsigned long long>^>(localSettings->Values->Lookup(L"expectedIndexRevision"));
        localSettings->Values->Insert(L"expectedIndexRevision", revisionNumber->Value + 1);
    }

    inline void OnIndexerOperationComplete(Windows::Storage::Search::ContentIndexer^ indexer)
    {
        // Set the expectedIndexRevision to be the reported index revision number because the operation
        // has succeeded
        auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
        localSettings->Values->Insert(L"expectedIndexRevision", indexer->Revision); 
    }

    // This function takes a string or vector string property value and returns a flattened string representation of it.
    inline Platform::String^ StringifyProperty(Platform::Object^ property)
    {
        Platform::String^ propertyString;
        // We need special handling to stringify vector properties such as the Keywords property.
        auto vectorProperty = dynamic_cast<Platform::IBoxArray<Platform::String^>^>(property);
        if (vectorProperty != nullptr)
        {
            for (const auto& str : vectorProperty->Value)
            {
                propertyString += str + L"; ";
            }
        }
        else
        {
            propertyString = property->ToString();
        }
        return propertyString;
    }

    // This function creates an instance of the ContentIndexer and inserts three items into the index with generic Key,
    // Name, Keywords, and Comment properties.
    inline void AddItemsToIndex(Windows::Storage::Search::ContentIndexer^ indexer)
    {
        const int numberOfItemsToIndex = 3;
        Concurrency::task<void> addItemTasks;
        for (int i = 0; i < numberOfItemsToIndex; i++)
        {
            auto content = ref new Windows::Storage::Search::IndexableContent();
            content->Id = L"SampleKey" + i;
            content->Properties->Insert(Windows::Storage::SystemProperties::ItemNameDisplay, L"Sample Item Name " + i);
            content->Properties->Insert(Windows::Storage::SystemProperties::Keywords, L"Sample keyword " + i);
            content->Properties->Insert(Windows::Storage::SystemProperties::Comment, L"Sample comment " + i);
            OnIndexerOperationBegin();
            auto nextTask = Concurrency::create_task(indexer->AddAsync(content));
            addItemTasks = (i == 0) ? nextTask : (addItemTasks && nextTask);
        }

        addItemTasks.then([indexer]()
        {
            OnIndexerOperationComplete(indexer);
            auto propertyKeys = ref new Platform::Collections::Vector<Platform::String^>();
            propertyKeys->Append(Windows::Storage::SystemProperties::ItemNameDisplay);
            auto query = indexer->CreateQuery(L"*", propertyKeys);
            return query->GetCountAsync();
        }).then([](unsigned int count)
        {
            MainPage::Current->NotifyUser(L"Number of items currently in the index: " + count, NotifyType::StatusMessage);
        });
    }
    
    inline Platform::String^ CreateItemString(Platform::String^ itemKey,
                                              Platform::Collections::Vector<Platform::String^>^ propertyKeys,
                                              Windows::Foundation::Collections::IMapView<Platform::String^, Platform::Object^>^ properties)
    {
        Platform::String^ itemString = L"Key: " + itemKey + L"\n";
        for (const auto& propkey : propertyKeys)
        {
            itemString += propkey + L": " + StringifyProperty(properties->Lookup(propkey)) + L"\n";
        }
        return itemString;
    }

    inline Platform::String^ HResultToString(int hr)
    {
        wchar_t hresultBuffer[11];
        swprintf_s(hresultBuffer, ARRAYSIZE(hresultBuffer), L"%X", hr);
        return ref new Platform::String(hresultBuffer);
    }

    // For the purposes of this sample, the appcontent-ms files are stored in an "appcontent-ms" folder in the
    // install directory. These are then copied into the app's "LocalState\Indexed" folder, which exposes them
    // to the indexer.
    inline void AddAppContentFilesToIndexedFolder()
    {
        auto localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
        auto installDirectory = Windows::ApplicationModel::Package::Current->InstalledLocation;
        auto outputStrings = ref new Platform::Collections::Vector<Platform::String^>();
        outputStrings->Append(L"Items added to the \"Indexed\" folder:");
        Concurrency::create_task(installDirectory->GetFolderAsync(L"appcontent-ms")).then(
        [outputStrings, localFolder](Windows::Storage::StorageFolder^ appContentFolder)
        {
            return Concurrency::create_task(appContentFolder->GetFilesAsync()).then(
            [outputStrings, localFolder] (Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile^>^ files)
            {
                return Concurrency::create_task(localFolder->CreateFolderAsync(L"Indexed", Windows::Storage::CreationCollisionOption::OpenIfExists)).then(
                [outputStrings, files](Windows::Storage::StorageFolder^ indexedFolder)
                {
                    std::vector<Concurrency::task<Windows::Storage::StorageFile^>> copyTasks;
                    for (const auto& file : files)
                    {
                        copyTasks.push_back(Concurrency::create_task(file->CopyAsync(indexedFolder, file->Name, Windows::Storage::NameCollisionOption::ReplaceExisting)));
                        outputStrings->Append(L"\n" + file->DisplayName + file->FileType);
                    }
                    return Concurrency::when_all(copyTasks.begin(), copyTasks.end());
                });
            });
        }).then([outputStrings](std::vector<Windows::Storage::StorageFile^> files)
        {
            auto outputString = ref new Platform::String(L"");
            for (const auto& str : outputStrings)
            {
                outputString += str;
            }
            MainPage::Current->NotifyUser(outputString, NotifyType::StatusMessage);
        });
    }
}
