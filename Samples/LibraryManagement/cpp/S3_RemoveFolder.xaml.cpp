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
#include "S3_RemoveFolder.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario3::Scenario3()
{
    InitializeComponent();
    GetPicturesLibrary();
}

/// <summary>
/// Requests the user's permission to remove the selected location from the Pictures library.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3::RemoveFolderButton_Click(Platform::Object^ /*sender*/, Windows::UI::Xaml::RoutedEventArgs^ /*e*/)
{
    auto folderToRemove = safe_cast<StorageFolder^>(FoldersComboBox->SelectedItem);
    create_task(picturesLibrary->RequestRemoveFolderAsync(folderToRemove)).then([this, folderToRemove](bool folderRemoved)
    {
        if (folderRemoved)
        {
            OutputTextBlock->Text = folderToRemove->DisplayName + " was removed from the Pictures library.";
        }
        else
        {
            OutputTextBlock->Text = "Operation canceled.";
        }
    });
}

/// <summary>
/// Gets the Pictures library and sets up the FoldersComboBox to list its folders.
/// </summary>
void Scenario3::GetPicturesLibrary()
{
    create_task(StorageLibrary::GetLibraryAsync(KnownLibraryId::Pictures)).then([this](StorageLibrary^ library)
    {
        picturesLibrary = library;

        // Bind the ComboBox to the list of folders currently in the library
        FoldersComboBox->ItemsSource = picturesLibrary->Folders;

        // Update the states of our controls when the list of folders changes
        picturesLibrary->DefinitionChanged += ref new TypedEventHandler<StorageLibrary^, Platform::Object^>(
            [this](StorageLibrary^ /*sender*/, Platform::Object^ /*e*/)
        {
            Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
            {
                UpdateControls();
            }));
        });

        UpdateControls();
    });
}

/// <summary>
// Fills the ComboBox control with the folders that make up the Pictures library.
/// </summary>
void Scenario3::FillComboBox()
{
    FoldersComboBox->ItemsSource = nullptr;
    FoldersComboBox->ItemsSource = picturesLibrary->Folders;
}

/// <summary>
/// Updates the Visibility and IsEnabled properties of UI controls that depend upon the Pictures library
/// having at least one folder in its Folders list.
/// </summary>
void Scenario3::UpdateControls()
{
    FillComboBox();
    bool libraryHasFolders = (picturesLibrary->Folders->Size > 0);
    FoldersComboBox->SelectedIndex = libraryHasFolders ? 0 : -1;
    FoldersComboBox->Visibility = libraryHasFolders ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
    LibraryEmptyTextBlock->Visibility = libraryHasFolders ? Windows::UI::Xaml::Visibility::Collapsed : Windows::UI::Xaml::Visibility::Visible;
    RemoveFolderButton->IsEnabled = libraryHasFolders;
}
