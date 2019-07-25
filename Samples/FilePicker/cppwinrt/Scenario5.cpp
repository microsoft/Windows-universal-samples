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
#include "Scenario5.h"
#include "Scenario5.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::AccessCache;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Provider;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario5::Scenario5()
    {
        InitializeComponent();
        UpdateButtons();
    }

    void Scenario5::UpdateButtons()
    {
        WriteToFileButton().IsEnabled(m_afterWriteFile != nullptr);
        WriteToFileWithCFUButton().IsEnabled(m_afterWriteFile != nullptr);

        SaveToFutureAccessListButton().IsEnabled(m_beforeReadFile != nullptr);
        GetFileFromFutureAccessListButton().IsEnabled(!m_faToken.empty());
    }

    fire_and_forget Scenario5::CreateFileButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        FileSavePicker savePicker;
        savePicker.SuggestedStartLocation(PickerLocationId::DocumentsLibrary);

        savePicker.FileTypeChoices().Insert(L"Plain Text", single_threaded_vector<hstring>({ L".txt" }));
        savePicker.SuggestedFileName(L"New Document");

        m_afterWriteFile = co_await savePicker.PickSaveFileAsync();
        if (m_afterWriteFile != nullptr)
        {
            rootPage.NotifyUser(L"File created.", NotifyType::StatusMessage);
        }
        else
        {
            rootPage.NotifyUser(L"Operation cancelled.", NotifyType::ErrorMessage);
        }
        UpdateButtons();
    }

    fire_and_forget Scenario5::WriteToFileButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        co_await FileIO::WriteTextAsync(m_afterWriteFile, L"The File Picker App just wrote to the file!");
        rootPage.NotifyUser(L"File write complete. If applicable, a WriteActivationMode.AfterWrite activation will occur in approximately 60 seconds on desktop.", NotifyType::StatusMessage);
    }

    fire_and_forget Scenario5::WriteToFileWithExplicitCFUButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        StorageFile file = m_afterWriteFile;
        CachedFileManager::DeferUpdates(file);
        co_await FileIO::WriteTextAsync(file, L"The File Picker App just wrote to the file!");
        rootPage.NotifyUser(L"File write complete. Explicitly completing updates.", NotifyType::StatusMessage);
        FileUpdateStatus status = co_await CachedFileManager::CompleteUpdatesAsync(file);
        switch (status)
        {
        case FileUpdateStatus::Complete:
            rootPage.NotifyUser(L"File " + file.Name() + L" was saved.", NotifyType::StatusMessage);
            break;

        case FileUpdateStatus::CompleteAndRenamed:
            rootPage.NotifyUser(L"File " + file.Name() + L" was renamed and saved.", NotifyType::StatusMessage);
            break;

        default:
            rootPage.NotifyUser(L"File " + file.Name() + L" couldn't be saved.", NotifyType::ErrorMessage);
            break;
        }
    }

    fire_and_forget Scenario5::PickAFileButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        FileOpenPicker openPicker;
        openPicker.ViewMode(PickerViewMode::Thumbnail);
        openPicker.FileTypeFilter().Append(L".txt");

        StorageFile file = co_await openPicker.PickSingleFileAsync();
        if (file != nullptr)
        {
            m_beforeReadFile = file;
            rootPage.NotifyUser(L"Picked file: " + file.Name(), NotifyType::StatusMessage);
        }
        else
        {
            m_beforeReadFile = nullptr;
            rootPage.NotifyUser(L"Operation cancelled.", NotifyType::ErrorMessage);
        }
        UpdateButtons();
    }

    void Scenario5::SaveToFutureAccessListButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        m_faToken = StorageApplicationPermissions::FutureAccessList().Add(m_beforeReadFile);
        rootPage.NotifyUser(L"Saved to Future Access List", NotifyType::StatusMessage);
        m_beforeReadFile = nullptr;
        UpdateButtons();

    }

    fire_and_forget Scenario5::GetFileFromFutureAccessListButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        rootPage.NotifyUser(L"Getting the file from the Future Access List. If applicable, a ReadActivationMode.BeforeAccess activation will occur in approximately 60 seconds on desktop.", NotifyType::StatusMessage);
        StorageFile file = co_await StorageApplicationPermissions::FutureAccessList().GetFileAsync(m_faToken);
        if (file != nullptr)
        {
            rootPage.NotifyUser(L"Retrieved file from Future Access List: " + file.Name(), NotifyType::StatusMessage);
            m_beforeReadFile = file;
        }
        else
        {
            rootPage.NotifyUser(L"Unable to retrieve file from Future Access List.", NotifyType::ErrorMessage);
        }
        UpdateButtons();

    }
}
