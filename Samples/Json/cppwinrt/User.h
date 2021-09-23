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

#pragma once
#include <winrt/SDKTemplate.h>
#include "User.g.h"

using namespace winrt;
using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Foundation::Collections;

namespace winrt::SDKTemplate::implementation
{
    struct User : UserT<User>
    {
        User() = default;
        User(hstring const& jsonString);

        hstring Id() { return m_id; }
        void Id(hstring const& value) { m_id = value; }
        hstring Phone() { return m_phone; }
        void Phone(hstring const& value) { m_phone = value; }
        hstring Name() { return m_name; }
        void Name(hstring const& value) { m_name = value; }
        Windows::Foundation::Collections::IObservableVector<SDKTemplate::School> Education() { return m_education; }
        double Timezone() { return m_timezone; }
        void Timezone(double value) { m_timezone = value; }
        bool Verified() { return m_verified; }
        void Verified(bool value) { m_verified = value; }
        hstring Stringify();

    private:
        static constexpr inline wchar_t idKey[] = L"id";
        static constexpr inline wchar_t phoneKey[] = L"phone";
        static constexpr inline wchar_t nameKey[] = L"name";
        static constexpr inline wchar_t educationKey[] = L"education";
        static constexpr inline wchar_t timezoneKey[] = L"timezone";
        static constexpr inline wchar_t verifiedKey[] = L"verified";

        hstring m_id;
        hstring m_phone;
        hstring m_name;
        Windows::Foundation::Collections::IObservableVector<winrt::SDKTemplate::School> m_education = single_threaded_observable_vector<SDKTemplate::School>();
        double m_timezone = 0.0;
        bool m_verified = false;
    };
}
namespace winrt::SDKTemplate::factory_implementation
{
    struct User : UserT<User, implementation::User>
    {
    };
}
