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
#include "Scenario6_Properties.xaml.h"
#include <sstream>

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

Scenario6::Scenario6()
{
    InitializeComponent();
}

void Scenario6::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage->ValidateFile();
}

void Scenario6::ShowPropertiesButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        struct State
        {
            std::wstringstream outputText;
            DateTimeFormatter^ dateFormatter = ref new DateTimeFormatter("shortdate longtime");
            String^ dateAccessedProperty = "System.DateAccessed";
            String^ fileOwnerProperty = "System.FileOwner";
        };

        // Get top level file properties
        auto state = std::make_shared<State>();
        state->outputText << L"File name: " << file->Name->Data() << std::endl;
        state->outputText << L"File type: " << file->FileType->Data() << std::endl;

        // Get basic properties
        create_task(file->GetBasicPropertiesAsync()).then([this, file, state](BasicProperties^ basicProperties)
        {
            state->outputText << L"File size: " << basicProperties->Size << L" bytes" << std::endl;
            state->outputText << L"Date modified: " << state->dateFormatter->Format(basicProperties->DateModified)->Data() << std::endl;

            // Get extra properties
            auto propertiesName = ref new Vector<String^>({ state->dateAccessedProperty , state->fileOwnerProperty });
            return file->Properties->RetrievePropertiesAsync(propertiesName);
        }).then([this, state](IMap<String^, Object^>^ extraProperties)
        {
            auto propValue = extraProperties->Lookup(state->dateAccessedProperty);
            if (propValue != nullptr)
            {
                DateTime dateAccessed = (DateTime)propValue;
                state->outputText << L"Date accessed: " << state->dateFormatter->Format(dateAccessed)->Data() << std::endl;
            }
            propValue = extraProperties->Lookup(state->fileOwnerProperty);
            if (propValue != nullptr)
            {
                state->outputText << L"File owner: " << ((String^)propValue)->Data();
            }

            rootPage->NotifyUser(ref new String(state->outputText.str().c_str()), NotifyType::StatusMessage);
        }).then([this, file](task<void> task)
        {
            try
            {
                task.get();
            }
            catch (COMException^ ex)
            {
                // I/O errors are reported as exceptions.
                rootPage->HandleIoException(ex, "Error retrieving properties for '" + file->Name + "'");
            }
        });
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}
