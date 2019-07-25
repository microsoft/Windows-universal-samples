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
#include "Scenario6.h"
#include "Scenario6.g.cpp"
#include "SampleConfiguration.h"
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario6::Scenario6()
    {
        InitializeComponent();
    }

    void Scenario6::OnNavigatedTo(NavigationEventArgs const&)
    {
        SampleState::ValidateFile();
    }

    fire_and_forget Scenario6::ShowPropertiesButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        DateTimeFormatter dateFormatter{ L"shortdate longtime" };

        StorageFile file = SampleState::SampleFile();
        if (file != nullptr)
        {
            std::wstringstream stream;

            try
            {
                // Get top level file properties
                stream << L"File name: " << std::wstring_view{ file.Name() } << std::endl;
                stream << L"File type: " << std::wstring_view{ file.FileType() } << std::endl;

                // Get basic properties
                BasicProperties basicProperties = co_await file.GetBasicPropertiesAsync();
                stream << L"File size: " << basicProperties.Size() << std::endl;
                stream << L"Date modified: " << std::wstring_view{ dateFormatter.Format(basicProperties.DateModified()) } << std::endl;

                // Get extra properties
                hstring dateAccessProperty{ L"System.DateAccessed" };
                hstring fileOwnerProperty{ L"System.FileOwner" };
                IMap<hstring, IInspectable> extraProperties = co_await file.Properties().RetrievePropertiesAsync({ dateAccessProperty, fileOwnerProperty });

                auto accessDate = extraProperties.TryLookup(dateAccessProperty);
                if (accessDate) {
                    stream << L"Date accessed: " << std::wstring_view{ dateFormatter.Format(unbox_value<DateTime>(accessDate)) } << std::endl;
                }

                auto fileOwner = extraProperties.TryLookup(fileOwnerProperty);
                if (fileOwner) {
                    stream << L"File owner: " << std::wstring_view{ unbox_value<hstring>(fileOwner) } << std::endl;
                }

                rootPage.NotifyUser(stream.str(), NotifyType::StatusMessage);
            }
            catch (const hresult_error& ex)
            {
                // I/O errors are reported as exceptions.
                rootPage.NotifyUser(L"Error retrieving properties for '" + file.Name() + L"': " + ex.message(), NotifyType::ErrorMessage);
            }
        }
        else
        {
            SampleState::NotifyUserFileNotExist();
        }
    }
}
