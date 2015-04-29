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

//
// ShareFiles.xaml.cpp
// Implementation of the ShareFiles class
//

#include "pch.h"
#include "ShareFiles.xaml.h"
#include <sstream>

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace concurrency;
using namespace Windows::ApplicationModel::DataTransfer;

ShareFiles::ShareFiles()
{
    InitializeComponent();
}

bool ShareFiles::GetShareContent(DataRequest^ request)
{
    bool succeeded = false;

    if (storageItems != nullptr)
    {
        auto requestData = request->Data;
        requestData->Properties->Title = TitleInputBox->Text;

        // The description is optional.
        auto dataPackageDescription = DescriptionInputBox->Text;
        if (dataPackageDescription != nullptr)
        {
            requestData->Properties->Description = dataPackageDescription;
        }
        requestData->SetStorageItems(dynamic_cast<IIterable<IStorageItem^>^>(storageItems));
        succeeded = true;
    }
    else
    {
        request->FailWithDisplayText("Select the files you would like to share and try again.");
    }
    return succeeded;
}

void ShareFiles::SelectFilesButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto filePicker = ref new FileOpenPicker();
    filePicker->ViewMode = PickerViewMode::List;
    filePicker->SuggestedStartLocation = PickerLocationId::DocumentsLibrary;
    filePicker->FileTypeFilter->Append("*");

    create_task(filePicker->PickMultipleFilesAsync()).then([this](IVectorView<StorageFile^>^ pickedFiles)
    {
        if (pickedFiles->Size > 0)
        {
            storageItems = pickedFiles;

            // Display the file names in the UI.
            std::wstringstream sstream;
            sstream << L"Picked files: ";
            for (unsigned int i = 0; i < pickedFiles->Size; i++)
            {
                sstream << pickedFiles->GetAt(i)->Name->Data();
                if (i < pickedFiles->Size - 1)
                {
                    sstream << L", ";
                }
            }
            sstream << L".";
            MainPage::Current->NotifyUser(ref new String(sstream.str().c_str()), NotifyType::StatusMessage);

            ShareStep->Visibility = Windows::UI::Xaml::Visibility::Visible;
        }
    });
}
