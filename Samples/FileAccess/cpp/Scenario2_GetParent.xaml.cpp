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
#include "Scenario2_GetParent.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

Scenario2::Scenario2()
{
    InitializeComponent();
}

void Scenario2::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage->ValidateFile();
}

void Scenario2::GetParent_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = rootPage->SampleFile;
    if (file != nullptr)
    {
        create_task(file->GetParentAsync()).then([this, file](StorageFolder^ parentFolder)
        {
            if (parentFolder != nullptr)
            {
                String^ outputText = "Item: " + file->Name + " (" + file->Path + ")";
                outputText += "\nParent: " + parentFolder->Name + " (" + parentFolder->Path + ")";
                rootPage->NotifyUser(outputText, NotifyType::StatusMessage);
            }
        });
    }
    else
    {
        rootPage->NotifyUserFileNotExist();
    }
}
