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
#include "CopyFiles.h"
#include "CopyFiles.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::SDKTemplate::implementation
{
    CopyFiles::CopyFiles()
    {
        InitializeComponent();
    }

    fire_and_forget CopyFiles::CopyButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        FileOpenPicker filePicker;
        filePicker.ViewMode(PickerViewMode::List);
        filePicker.FileTypeFilter().ReplaceAll({ L"*" });

        auto storageItems = co_await filePicker.PickMultipleFilesAsync();
        if (storageItems.Size() > 0)
        {
            DataPackage dataPackage;
            dataPackage.SetStorageItems(storageItems.as<IVectorView<IStorageItem>>());

            // Request a copy operation from targets that support different file operations, like File Explorer
            dataPackage.RequestedOperation(DataPackageOperation::Copy);
            if (Clipboard::SetContentWithOptions(dataPackage, nullptr))
            {
                rootPage.NotifyUser(L"Storage Items: " + to_hstring(storageItems.Size()) + L" file(s) are copied into clipboard", NotifyType::StatusMessage);
            }
            else
            {
                // Copying data to Clipboard can potentially fail - for example, if another application is holding Clipboard open
                rootPage.NotifyUser(L"Error copying content to clipboard.", NotifyType::ErrorMessage);
            }
        }
    }

    fire_and_forget CopyFiles::PasteButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        // Get data package from clipboard
        auto dataPackageView = Clipboard::GetContent();
        if (dataPackageView.Contains(StandardDataFormats::StorageItems()))
        {
            IVectorView<IStorageItem> storageItems = nullptr;
            try
            {
                storageItems = co_await dataPackageView.GetStorageItemsAsync();
            }
            catch (hresult_error const& ex)
            {
                rootPage.NotifyUser(L"Error retrieving file(s) from Clipboard: " + ex.message(), NotifyType::ErrorMessage);
            }

            if (storageItems)
            {
                std::wostringstream output;
                output << L"Pasting following " << storageItems.Size() <<
                    L" file(s) to the folder " <<
                    std::wstring_view(ApplicationData::Current().LocalFolder().Path()) << std::endl;
                output << L"Requested Operation: ";
                auto operation = dataPackageView.RequestedOperation();
                switch (operation)
                {
                case DataPackageOperation::Copy:
                    output << L"Copy";
                    break;
                case DataPackageOperation::Link:
                    output << L"Link";
                    break;
                case DataPackageOperation::Move:
                    output << L"Move";
                    break;
                case DataPackageOperation::None:
                    output << L"None";
                    break;
                default:
                    output << L"Unknown";
                    break;
                }
                output << std::endl;
                OutputText().Text(output.str());

                // Iterate through each item in the collection
                for (auto&& storageItem : storageItems)
                {
                    auto file = storageItem.try_as<StorageFile>();
                    if (file)
                    {
                        // Copy the file
                        auto newFile = co_await file.CopyAsync(ApplicationData::Current().LocalFolder(), file.Name(), NameCollisionOption::ReplaceExisting);
                        if (newFile)
                        {
                            output << std::wstring_view(file.Name()) << std::endl;
                        }
                    }
                    else
                    {
                        auto folder = storageItem.try_as<StorageFolder>();
                        if (folder)
                        {
                            // Skipping folders for brevity sake
                            output << std::wstring_view(folder.Path()) << L" is a folder, skipping" << std::endl;
                        }
                    }
                    ;
                    OutputText().Text(output.str());
                }
            }
        }
        else
        {
            OutputText().Text(L"StorageItems format is not available in clipboard");
        }
    }
}

