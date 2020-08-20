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
#include "Scenario10_SetCookie.h"
#include "Scenario10_SetCookie.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;

namespace winrt::SDKTemplate::implementation
{
    Scenario10_SetCookie::Scenario10_SetCookie()
    {
        InitializeComponent();
    }

    void Scenario10_SetCookie::SetCookie_Click(IInspectable const&, RoutedEventArgs const&)
    {
        hstring name = NameField().Text();
        hstring domain = DomainField().Text();
        hstring path = PathField().Text();

        if (name.empty() || domain.empty() || path.empty())
        {
            rootPage.NotifyUser(L"Invalid name, domain, or path value.", NotifyType::ErrorMessage);
            return;
        }

        try
        {
            HttpCookie cookie(name, domain, path);
            cookie.Value(ValueField().Text());

            if (NullCheckBox().IsChecked().Value())
            {
                cookie.Expires(nullptr);
            }
            else
            {
                cookie.Expires(ExpiresDatePicker().Date() + ExpiresTimePicker().Time());
            }

            cookie.Secure(SecureToggle().IsOn());
            cookie.HttpOnly(HttpOnlyToggle().IsOn());

            HttpBaseProtocolFilter filter;
            bool replaced = filter.CookieManager().SetCookie(cookie, false);

            if (replaced)
            {
                rootPage.NotifyUser(L"Cookie replaced.", NotifyType::StatusMessage);
            }
            else
            {
                rootPage.NotifyUser(L"Cookie set.", NotifyType::StatusMessage);
            }
        }
        catch (hresult_invalid_argument const& ex)
        {
            rootPage.NotifyUser(ex.message(), NotifyType::StatusMessage);
        }
        catch (hresult_error const& ex)
        {
            rootPage.NotifyUser(L"Error: " + ex.message(), NotifyType::ErrorMessage);
        }
    }
}
