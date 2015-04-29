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
// S4_CheckIndexRevision.xaml.cpp
// Implementation of the S4_CheckIndexRevision class
//

#include "pch.h"
#include "S4_CheckIndexRevision.xaml.h"
#include "MainPage.xaml.h"
#include "Helpers.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

S4_CheckIndexRevision::S4_CheckIndexRevision()
{
    InitializeComponent();

    // This initializes the value used throughout the sample which tracks the expected index revision number. This
    // value is used to check if the app's expected revision number matches the actual index revision number and is
    // stored in the app's local settings to allow it to persist across termination.
    auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
    if (!localSettings->Values->HasKey(L"expectedIndexRevision"))
    {
        localSettings->Values->Insert(L"expectedIndexRevision", ref new Platform::Box<unsigned long long>(0));
    }
}

// This function displays the app's expected index revision number and the actual index revision number reported by
// the index. For purposes of this sample, this is done in reaction to a button being pressed. In practice, this should
// be done when an app is activated.
// If the reported index revision number is 0, it means the index has been reset and an app should re-push all of its
// data.
// If the reported index revision number is not 0, but doesn't match the app's expected index revision number, it
// indicates that not all of the app's adds/deletes/updates were successfully processed. This should be an edge case,
// but when hit, the app should re-do the missed operations. Tracking the expected index revision number can be used to
// create checkpoints in the app so it can only re-do the operations required to match the expected index revision
// number.
void S4_CheckIndexRevision::CheckIndexRevision_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto indexer = Windows::Storage::Search::ContentIndexer::GetIndexer();
    auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
    String^ outputString = L"Expected Index Revision Number: " + localSettings->Values->Lookup(L"expectedIndexRevision");
    outputString += L"\nReported Index Revision Number: " + indexer->Revision;
    if (localSettings->Values->Lookup(L"expectedIndexRevision") != indexer->Revision)
    {
        // There is a mismatch between the expected and reported index revision numbers.
        if (indexer->Revision == 0)
        {
            // The index has been reset, so code would be added here to re-push all data.
        }
        else
        {
            // The index hasn't been reset, but it doesn't contain all expected updates, so add code to get the index
            // back into the expected state.
        }

        // After doing the necessary work to get back to a synchronized state, set the expected index revision number
        // to match the reported revision number.
        localSettings->Values->Insert(L"expectedIndexRevision", indexer->Revision);
    }
    MainPage::Current->NotifyUser(outputString, NotifyType::StatusMessage);
}
