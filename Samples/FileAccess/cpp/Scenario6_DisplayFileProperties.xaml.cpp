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
#include "Scenario6_DisplayFileProperties.xaml.h"

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

Scenario6::Scenario6() : rootPage(MainPage::Current)
{
    InitializeComponent();
    rootPage->Initialize();
    rootPage->ValidateFile();
    ShowPropertiesButton->Click += ref new RoutedEventHandler(this, &Scenario6::ShowPropertiesButton_Click);
}

DateTimeFormatter^ Scenario6::dateFormat = ref new DateTimeFormatter("shortdate");
DateTimeFormatter^ Scenario6::timeFormat = ref new DateTimeFormatter("longtime");
String^ Scenario6::dateAccessedProperty = "System.DateAccessed";
String^ Scenario6::fileOwnerProperty    = "System.FileOwner";

void Scenario6::ShowPropertiesButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        // Get top level file properties
        auto outputText = std::make_shared<String^>("File name: " + file->Name);
        *outputText += "\nFile type: " + file->FileType;

        // Get basic properties
        create_task(file->GetBasicPropertiesAsync()).then([this, file, outputText](BasicProperties^ basicProperties)
        {
            *outputText += "\nFile size: " + basicProperties->Size.ToString() + " bytes";

            String^ dateModifiedString = dateFormat->Format(basicProperties->DateModified) + " " + timeFormat->Format(basicProperties->DateModified);
            *outputText += "\nDate modified: " + dateModifiedString;

            // Get extra properties
            auto propertiesName = ref new Vector<String^>();
            propertiesName->Append(dateAccessedProperty);
            propertiesName->Append(fileOwnerProperty);
            return file->Properties->RetrievePropertiesAsync(propertiesName);
        }).then([this, outputText](IMap<String^, Object^>^ extraProperties)
        {
            auto propValue = extraProperties->Lookup(dateAccessedProperty);
            if (propValue != nullptr)
            {
                DateTime dateAccessed = (DateTime)propValue;
                *outputText += "\nDate accessed: " + dateFormat->Format(dateAccessed) + " " + timeFormat->Format(dateAccessed);
            }
            propValue = extraProperties->Lookup(fileOwnerProperty);
            if (propValue != nullptr)
            {
                *outputText += "\nFile owner: " + propValue;
            }

            rootPage->NotifyUser(*outputText, NotifyType::StatusMessage);
        }).then([this, file](task<void> task)
        {
            try
            {
                task.get();
            }
            catch (COMException^ ex)
            {
                rootPage->HandleIoException(ex, "Error retrieving properties for '" + file->Name + "'");
            }
        });
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}
