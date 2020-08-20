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
#include "Scenario11_DeleteCookie.h"
#include "Scenario11_DeleteCookie.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;

namespace winrt::SDKTemplate::implementation
{
    Scenario11_DeleteCookie::Scenario11_DeleteCookie()
    {
        InitializeComponent();
    }

    void Scenario11_DeleteCookie::DeleteCookie_Click(IInspectable const&, RoutedEventArgs const&)
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

            HttpBaseProtocolFilter filter;
            filter.CookieManager().DeleteCookie(cookie);

            rootPage.NotifyUser(L"Cookie deleted.", NotifyType::StatusMessage);
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
