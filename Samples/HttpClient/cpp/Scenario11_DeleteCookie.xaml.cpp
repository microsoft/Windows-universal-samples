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
#include "Scenario11_DeleteCookie.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;

Scenario11::Scenario11()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario11::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

void Scenario11::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
    {
        return;
    }

    Page::OnNavigatedFrom(e);
}

void Scenario11::DeleteCookie_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (NameField->Text->IsEmpty() || DomainField->Text->IsEmpty() || PathField->Text->IsEmpty())
    {
        rootPage->NotifyUser("Invalid name, domain, or path value.", NotifyType::ErrorMessage);
        return;
    }

    try
    {
        HttpCookie^ cookie = ref new HttpCookie(NameField->Text, DomainField->Text, PathField->Text);

        HttpBaseProtocolFilter^ filter = ref new HttpBaseProtocolFilter();
        filter->CookieManager->DeleteCookie(cookie);

        rootPage->NotifyUser("Cookie deleted.", NotifyType::StatusMessage);
    }
    catch (InvalidArgumentException^ ex)
    {
        rootPage->NotifyUser(ex->Message, NotifyType::StatusMessage);
    }
    catch (Exception^ ex)
    {
        rootPage->NotifyUser("Error: " + ex->Message, NotifyType::ErrorMessage);
    }
}
