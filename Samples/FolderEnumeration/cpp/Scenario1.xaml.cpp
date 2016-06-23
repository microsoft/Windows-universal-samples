//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario1.xaml.cpp
// Implementation of the Scenario1 class
//

#include "pch.h"
#include "Scenario1.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario1::Scenario1()
{
    InitializeComponent();
}

void Scenario1::GetFilesAndFoldersButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    OutputTextBlock->Text = "";

    create_task(KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary)).then([this](StorageFolder^ picturesFolder)
    {
        create_task(picturesFolder->GetFoldersAsync()).then([this, picturesFolder](IVectorView<StorageFolder^>^ folders)
        {
            create_task(picturesFolder->GetFilesAsync()).then([this, picturesFolder, folders](IVectorView<StorageFile^>^ files)
            {
                auto count = folders->Size + files->Size;
                String^ outputText = ref new String();
                outputText = picturesFolder->Name + " (" + count.ToString() + ")\n\n";
                std::for_each(begin(folders), end(folders), [this, &outputText](StorageFolder^ folder)
                {
                    outputText += "    " + folder->DisplayName + "\\\n";
                });
                std::for_each(begin(files), end(files), [this, &outputText](StorageFile^ file)
                {
                    outputText += "    " + file->Name + "\n";
                });
                OutputTextBlock->Text = outputText;
            });
        });
    });
}
