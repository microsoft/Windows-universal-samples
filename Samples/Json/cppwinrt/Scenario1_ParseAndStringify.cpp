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
#include "Scenario1_ParseAndStringify.h"
#include "Scenario1_ParseAndStringify.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_ParseAndStringify::Scenario1_ParseAndStringify()
    {
        InitializeComponent();
        CurrentUser(SDKTemplate::User(JsonInput().Text()));
    }

    void Scenario1_ParseAndStringify::Parse_Click(IInspectable const&, RoutedEventArgs const&)
    {
        try
        {
            CurrentUser(SDKTemplate::User(JsonInput().Text()));
            rootPage.NotifyUser(L"JSON string parsed successfully.", NotifyType::StatusMessage);
        }
        catch (hresult_error const& ex)        
        {
            if (!IsExceptionHandled(ex))
            {
                throw;
            }
        }
    }

    void Scenario1_ParseAndStringify::Stringify_Click(IInspectable const&, RoutedEventArgs const&)
    {
        JsonInput().Text(L"");

        JsonInput().Text(CurrentUser().Stringify());
        rootPage.NotifyUser(L"JSON object serialized to string successfully.", NotifyType::StatusMessage);
    }

    void Scenario1_ParseAndStringify::Add_Click(IInspectable const&, RoutedEventArgs const&)
    {
        CurrentUser().Education().Append(SDKTemplate::School());
        rootPage.NotifyUser(L"New row added.", NotifyType::StatusMessage);
    }

    void Scenario1_ParseAndStringify::DeleteSchool_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto school = sender.as<FrameworkElement>().DataContext().as<SDKTemplate::School>();
        auto schools = CurrentUser().Education();
        uint32_t index;
        if (schools.IndexOf(school, index))
        {
            schools.RemoveAt(index);
        }
    }

    bool Scenario1_ParseAndStringify::IsExceptionHandled(hresult_error const& ex)
    {
        if (ex.code() == E_ILLEGAL_METHOD_CALL)
        {
            rootPage.NotifyUser(L"JSON did not match expected schema: " + ex.message(), NotifyType::ErrorMessage);
            return true;
        }

        hstring text;
        switch (JsonError::GetJsonStatus(ex.code()))
        {
        default:
        case JsonErrorStatus::Unknown: return false;
        case JsonErrorStatus::InvalidJsonString: text = L"Invalid JSON string"; break;
        case JsonErrorStatus::InvalidJsonNumber: text = L"Invalid JSON number"; break;
        case JsonErrorStatus::JsonValueNotFound: text = L"JSON value not found"; break;
        case JsonErrorStatus::ImplementationLimit: text = L"Implementation limit exceeded"; break;
        }

        rootPage.NotifyUser(text + L": " + ex.message(), NotifyType::ErrorMessage);
        return true;
    }
}
