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
// Scenario3.xaml.cpp
// Implementation of the Scenario3 class
//

#include "pch.h"
#include "Scenario3.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario3::Scenario3()
{
    InitializeComponent();
    rootPage = MainPage::Current;
    GetThumbnailButton->Click += ref new RoutedEventHandler(this, &Scenario3::GetThumbnailButton_Click);
}

void Scenario3::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage->ResetOutput(ThumbnailImage, OutputTextBlock, nullptr);
}

void Scenario3::GetThumbnailButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->ResetOutput(ThumbnailImage, OutputTextBlock, nullptr);

    // Pick a document
    FileOpenPicker^ openPicker = ref new FileOpenPicker();
    Array<String^>^ extensions = FileExtensions::Document;
    for (unsigned int i = 0; i < extensions->Length; i++)
    {
        openPicker->FileTypeFilter->Append(extensions[i]);
    }

    create_task(openPicker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            const ThumbnailMode thumbnailMode = ThumbnailMode::DocumentsView;
            const size_t size = 100;
            create_task(file->GetThumbnailAsync(thumbnailMode, size)).then([this, file, size](StorageItemThumbnail^ thumbnail)
            {
                if (thumbnail != nullptr)
                {
                    MainPage::DisplayResult(ThumbnailImage, OutputTextBlock, "DocumentsView", size, file, thumbnail, false);
                }
                else
                {
                    rootPage->NotifyUser(Errors::NoIcon, NotifyType::StatusMessage);
                }
            });
        }
        else
        {
            rootPage->NotifyUser(Errors::Cancel, NotifyType::StatusMessage);
        }
    });
}
