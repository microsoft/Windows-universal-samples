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
#include "Scenario2.h"
#include "Scenario2.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2::Scenario2()
    {
        InitializeComponent();
    }

    void Scenario2::OnNavigatedTo(NavigationEventArgs const&)
    {
        SampleState::ValidateFile();
    }

    fire_and_forget Scenario2::GetParent_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        StorageFile file = SampleState::SampleFile();
        if (file != nullptr)
        {
            try
            {
                StorageFolder parentFolder = await file.GetParentAsync();
                if (parentFolder != nullptr)
                {
                    hstring message = L"Item: " + file.Name() + L" (" + file.Path() + L")\n"
                        + L"Parent: " + parentFolder.Name() + L" (" + parentFolder.Path() + L")";
                    rootPage.NotifyUser(message, NotifyType::StatusMessage);
                }
            }
            catch (const hresult_error& ex)
            {
                if (ex.code() == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                {
                    SampleState::NotifyUserFileNotExist();
                }
                else
                {
                    throw;
                }
            }
        }
        else
        {
            SampleState::NotifyUserFileNotExist();
        }

    }
}
