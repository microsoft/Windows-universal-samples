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
#include "School.h"
#include "School.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    School::School(winrt::Windows::Data::Json::JsonObject const& jsonObject)
    {
        JsonObject schoolObject = jsonObject.GetNamedObject(schoolKey, nullptr);
        if (schoolObject != nullptr)
        {
            m_id = schoolObject.GetNamedString(idKey, L"");
            m_name = schoolObject.GetNamedString(nameKey, L"");
        }
        m_type = jsonObject.GetNamedString(typeKey);
    }

    winrt::Windows::Data::Json::JsonObject School::ToJsonObject()
    {
        JsonObject schoolObject;        
        schoolObject.SetNamedValue(idKey, JsonValue::CreateStringValue(m_id));
        schoolObject.SetNamedValue(nameKey, JsonValue::CreateStringValue(m_name));

        JsonObject jsonObject;
        // JsonObject can also be treated as an IMap.
        jsonObject.Insert(schoolKey, schoolObject);
        jsonObject.Insert(typeKey, JsonValue::CreateStringValue(m_type));

        return jsonObject;
    }
}
