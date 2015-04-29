//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// CopyFiles.xaml.cpp
// Implementation of the CopyFiles class
//

#include "pch.h"
#include "CopyFiles.xaml.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Media::Imaging;

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;

CopyFiles::CopyFiles()
{
    InitializeComponent();
}

void CopyFiles::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}


void CopyFiles::CopyButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    OutputText->Text = "";

    // Select a file using the filepicker.
    auto filePicker = ref new FileOpenPicker();
    filePicker->ViewMode = PickerViewMode::List;
    filePicker->FileTypeFilter->Append("*");

    create_task(filePicker->PickMultipleFilesAsync()).then(
        [this](IVectorView<StorageFile^>^ storageFiles)
    {
        if (storageFiles->Size > 0)
        {
            auto dataPackage = ref new DataPackage();
            auto items = safe_cast<IVectorView<IStorageItem^>^>(storageFiles);

            // Add picked files to DataPackage. Creates Read Only storage items by default.
            dataPackage->SetStorageItems(items);

            // Request a copy operation from targets that support different file operations, like File Explorer.
            dataPackage->RequestedOperation = DataPackageOperation::Copy;

            try
            {
                // Set the contents in clipboard
                DataTransfer::Clipboard::SetContent(dataPackage);
                rootPage->NotifyUserBackgroundThread("Storage Items: Total " + storageFiles->Size + " file(s) copied to the clipboard \n", NotifyType::StatusMessage);
            }
            catch (Exception^ ex)
            {
                // Copying data to Clipboard can potentially fail - for example, if another application is holding Clipboard open
                rootPage->NotifyUserBackgroundThread("Error copying storage items to clipboard: " + ex->Message + ".", NotifyType::ErrorMessage);
            }
        }
        else
        {
            rootPage->NotifyUserBackgroundThread("No file was selected \n", NotifyType::ErrorMessage);
        }
    });
}

void CopyFiles::PasteButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    OutputText->Text = "";

    // Get data package from clipboard
    DataPackageView^ dataPackageView = DataTransfer::Clipboard::GetContent();

    if (dataPackageView->Contains(StandardDataFormats::StorageItems))
    {
        create_task(dataPackageView->GetStorageItemsAsync()).then(
            [this, dataPackageView](task<IVectorView<IStorageItem^>^> itemsTask)
        {
            IVectorView<IStorageItem^>^ storageItems;
            try
            {
                storageItems = itemsTask.get();
            }
            catch (Exception^ ex)
            {
                rootPage->NotifyUserBackgroundThread("Failed GetStorageItemsAsync - " + ex->Message, NotifyType::ErrorMessage);
            }

            if (storageItems != nullptr)
            {
                rootPage->NotifyUserBackgroundThread("Pasting " + storageItems->Size + " file(s):\n", NotifyType::StatusMessage);

                OutputText->Text += "Requested Operation: ";

                switch (dataPackageView->RequestedOperation)
                {
                case DataPackageOperation::Copy:
                    {
                        OutputText->Text += "Copy \n\n";
                    }
                    break;

                case DataPackageOperation::Link:
                    {
                        OutputText->Text += "Link \n\n";
                    }
                    break;

                case DataPackageOperation::Move:
                    {
                        OutputText->Text += "Move \n\n";
                    }
                    break;

                case DataPackageOperation::None:
                    {
                        OutputText->Text += "None \n\n";
                    }
                    break;

                default:
                    break;
                }

                // Iterate through each element in the storageitem vector.
                std::for_each(begin(storageItems), end(storageItems),
                    [this](IStorageItem^ item)
                {
                    if (item->IsOfType(StorageItemTypes::File))
                    {
                        StorageFile^ file = safe_cast<StorageFile^>(item);

                        // Copy the file
                        create_task(file->CopyAsync(ApplicationData::Current->LocalFolder, file->Name, NameCollisionOption::ReplaceExisting)).then(
                            [=](task<StorageFile^> copiedTask)
                        {
                            StorageFile^ copiedFile;
                            try
                            {
                                copiedFile = copiedTask.get();
                            }
                            catch (Exception^ ex)
                            {
                                rootPage->NotifyUserBackgroundThread("Failed CopyAsync - " + ex->Message, NotifyType::ErrorMessage);
                            }

                            if (copiedFile)
                            {
                                OutputText->Text += "File: " + copiedFile->Name + " was copied from " + file->Name + "\n";
                            }
                        });
                    }
                    else
                    {
                        // Skipping folders for brevity sake.
                        OutputText->Text += item->Path + " is a folder, skipping \n";
                    }
                });
            }
        });
    }
    else
    {
        rootPage->NotifyUser("StorageItems format is not available in clipboard", NotifyType::StatusMessage);
    }
}

#pragma endregion
