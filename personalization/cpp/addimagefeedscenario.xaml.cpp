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
// AddImageFeedScenario.xaml.cpp
// Implementation of the AddImageFeedScenario class
//

#include "pch.h"
#include "AddImageFeedScenario.xaml.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::System::UserProfile;

AddImageFeedScenario::AddImageFeedScenario()
{
    InitializeComponent();
}

void AddImageFeedScenario::SetDefaultButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Pass a URI to your enclosure-based feed of lock screen images.  These must be JPG or PNG files.
    // Note that the sample URL given here is deliberately non-functional.
    create_task(LockScreen::RequestSetImageFeedAsync(ref new Uri("http://contoso.com/myimagerss.xml")))
    .then([this] (SetImageFeedResult result)
    {
        if (result == SetImageFeedResult::Success)
        {
            AddImageFeedScenarioOutputText->Text = "Called RequestSetImageFeedAsync: the specified URL was set as the default.";
        }
        else if (result == SetImageFeedResult::ChangeDisabled)
        {
            AddImageFeedScenarioOutputText->Text = "Called RequestSetImageFeedAsync: call succeeded but group policy has the lock screen image slide show turned off.";
        }
        else // (result == SetImageFeedResult.UserCanceled)
        {
            AddImageFeedScenarioOutputText->Text = "Called RequestSetImageFeedAsync: the specified URL was not set as the default.";
        }
    }, task_continuation_context::use_current());
}

void AddImageFeedScenario::RemoveFeedButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    bool fResult = LockScreen::TryRemoveImageFeed();
    if (fResult)
    {
        AddImageFeedScenarioOutputText->Text = "Called TryRemoveImageFeed: the associated URL registration was removed.";
    }
    else
    {
        AddImageFeedScenarioOutputText->Text = "Called TryRemoveImageFeed: the associated URL registration removal failed.";
    }
}
