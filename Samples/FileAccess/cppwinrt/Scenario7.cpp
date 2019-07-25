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
#include "Scenario7.h"
#include "Scenario7.g.cpp"
#include "SampleConfiguration.h"
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::AccessCache;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario7::Scenario7()
    {
        InitializeComponent();
    }

    void Scenario7::OnNavigatedTo(NavigationEventArgs const&)
    {
        SampleState::ValidateFile();
    }

    void Scenario7::AddToListButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StorageFile file = SampleState::SampleFile();
        if (file != nullptr)
        {
            if (MRURadioButton().IsChecked().Value()) {
                // Add the file to app MRU and possibly system MRU
                RecentStorageItemVisibility visibility = SystemMRUCheckBox().IsChecked().Value() ? RecentStorageItemVisibility::AppAndSystem : RecentStorageItemVisibility::AppOnly;
                SampleState::MruToken(StorageApplicationPermissions::MostRecentlyUsedList().Add(file, file.Name(), visibility));
                rootPage.NotifyUser(L"The file '" + file.Name() + L"' was added to the MRU list and a token was stored.", NotifyType::StatusMessage);
            }
            else
            {
                // Add the file to the FAL
                try
                {
                    SampleState::FalToken(StorageApplicationPermissions::FutureAccessList().Add(file, file.Name()));
                    rootPage.NotifyUser(L"The file '" + file.Name() + L"' was added to the FAL list and a token was stored.", NotifyType::StatusMessage);
                }
                catch (const hresult_error& ex)
                {
                    if (ex.code() == FA_E_MAX_PERSISTED_ITEMS_REACHED)
                    {
                        // A real program would call Remove() to create room in the FAL.
                        rootPage.NotifyUser(L"The file '" + file.Name() + L"' was not added to the FAL list because the FAL list is full.", NotifyType::ErrorMessage);
                    }
                    else
                    {
                        throw;
                    }
                }
            }
        }
        else
        {
            SampleState::NotifyUserFileNotExist();
        }
    }

    void Scenario7::ShowListButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        AccessListEntryView entries{ nullptr };
        std::wstring listName;
        if (MRURadioButton().IsChecked().Value())
        {
            listName = L"MRU";
            entries = StorageApplicationPermissions::MostRecentlyUsedList().Entries();
        }
        else
        {
            listName = L"FAL";
            entries = StorageApplicationPermissions::FutureAccessList().Entries();
        }

        std::wstringstream stream;
        if (entries.Size() > 0)
        {
            stream << L"The " << listName << L" list contains the following item(s):";
            for (AccessListEntry const& entry : entries)
            {
                stream << std::endl;
                // Application previously chose to store file.Name in this field
                stream << std::wstring_view{ entry.Metadata };
            }
        }
        else
        {
            stream << L"The " << listName << L" list is empty.";
        }
        rootPage.NotifyUser(stream.str(), NotifyType::StatusMessage);
    }

    fire_and_forget Scenario7::OpenFromListButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        StorageFile file{ nullptr };

        if (MRURadioButton().IsChecked().Value())
        {
            if (!SampleState::MruToken().empty())
            {
                // When the MRU becomes full, older entries are automatically deleted, so check if the
                // token is still valid before using it.
                if (StorageApplicationPermissions::MostRecentlyUsedList().ContainsItem(SampleState::MruToken()))
                {
                    // Open the file via the token that was stored when adding this file into the MRU list
                    file = co_await StorageApplicationPermissions::MostRecentlyUsedList().GetFileAsync(SampleState::MruToken());
                }
                else
                {
                    rootPage.NotifyUser(L"The token is no longer valid.", NotifyType::ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser(L"This operation requires a token. Add file to the MRU list first.", NotifyType::ErrorMessage);
            }
        }
        else
        {
            if (!SampleState::FalToken().empty())
            {
                // Open the file via the token that was stored when adding this file into the FAL list.
                // The token remains valid until we explicitly remove it.
                file = co_await StorageApplicationPermissions::FutureAccessList().GetFileAsync(SampleState::FalToken());
            }
            else
            {
                rootPage.NotifyUser(L"This operation requires a token. Add file to the FAL list first.", NotifyType::ErrorMessage);
            }
        }

        if (file != nullptr)
        {
            try
            {
                // Read the file
                hstring fileContent = co_await FileIO::ReadTextAsync(file);
                rootPage.NotifyUser(L"The file '" + file.Name() + L"' was opened by a stored token. It contains the following text:\n" + fileContent, NotifyType::StatusMessage);
            }
            catch (hresult_error const& ex)
            {
                // I/O errors are reported as exceptions.
                rootPage.NotifyUser(L"Error reading file opened from list: " + ex.message(), NotifyType::ErrorMessage);
            }
        }
    }
}
