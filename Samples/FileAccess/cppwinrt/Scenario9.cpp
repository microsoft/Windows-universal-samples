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
#include "Scenario9.h"
#include "Scenario9.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario9::Scenario9()
    {
        InitializeComponent();
    }

    void Scenario9::OnNavigatedTo(NavigationEventArgs const&)
    {
        SampleState::ValidateFile();
    }

    fire_and_forget Scenario9::CompareFilesButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        StorageFile file = SampleState::SampleFile();
        if (file != nullptr)
        {
            FileOpenPicker picker;
            picker.SuggestedStartLocation(PickerLocationId::PicturesLibrary);
            picker.FileTypeFilter().Append(L"*");
            StorageFile comparand = co_await picker.PickSingleFileAsync();
            if (comparand != nullptr)
            {
                try
                {
                    if (file.IsEqual(comparand))
                    {
                        rootPage.NotifyUser(L"Files are equal.", NotifyType::StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser(L"Files are not equal.", NotifyType::StatusMessage);
                    }
                }
                catch (const hresult_error& ex)
                {
                    // I/O errors are reported as exceptions.
                    rootPage.NotifyUser(L"Error comparing files: " + ex.message(), NotifyType::ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser(L"Operation cancelled", NotifyType::StatusMessage);
            }
        }
        else
        {
            SampleState::NotifyUserFileNotExist();
        }
    }
}
