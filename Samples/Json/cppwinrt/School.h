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
#include "School.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct School : SchoolT<School>
    {
        School() = default;
        School(winrt::Windows::Data::Json::JsonObject const& jsonObject);

        winrt::Windows::Data::Json::JsonObject ToJsonObject();

        hstring Id() { return m_id; }
        void Id(hstring const& value) { m_id = value; }
        hstring Name() { return m_name; }
        void Name(hstring const& value) { m_name = value; }
        hstring Type() { return m_type; }
        void Type(hstring const& value) { m_type = value; }

    private:
        static constexpr inline wchar_t idKey[] = L"id";
        static constexpr inline wchar_t schoolKey[] = L"school";
        static constexpr inline wchar_t nameKey[] = L"name";
        static constexpr inline wchar_t typeKey[] = L"type";

        hstring m_id;
        hstring m_name;
        hstring m_type;
    };
}
namespace winrt::SDKTemplate::factory_implementation
{
    struct School : SchoolT<School, implementation::School>
    {
    };
}
