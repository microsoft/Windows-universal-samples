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
#include "GetUserDisplayName.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System::UserProfile;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

GetUserDisplayName::GetUserDisplayName() : rootPage(SDKTemplate::MainPage::Current)
{
    InitializeComponent();
}

void GetUserDisplayName::GetDisplayNameButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (UserInformation::NameAccessAllowed)
    {
        create_task(UserInformation::GetDisplayNameAsync()).then([this](String^ displayName)
        {
            if (displayName != nullptr)
            {
                rootPage->NotifyUser("Display name = " + displayName, NotifyType::StatusMessage);
            }
            else
            {
                rootPage->NotifyUser("No display name was returned.", NotifyType::ErrorMessage);
            }
        });
    }
    else
    {
        rootPage->NotifyUser("Access to name disabled by Privacy Setting or Group Policy.", NotifyType::ErrorMessage);
    }
}
