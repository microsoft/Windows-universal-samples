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
#include "User.h"
#include "User.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Data::Json;

namespace winrt::SDKTemplate::implementation
{
    User::User(hstring const& jsonString)
    {
        JsonObject jsonObject = JsonObject::Parse(jsonString);
        m_id = jsonObject.GetNamedString(idKey, L"");

        IJsonValue phoneJsonValue = jsonObject.GetNamedValue(phoneKey);
        if (phoneJsonValue.ValueType() == JsonValueType::Null)
        {
            m_phone = {};
        }
        else
        {
            m_phone = phoneJsonValue.GetString();
        }

        m_name = jsonObject.GetNamedString(nameKey, L"");
        m_timezone = jsonObject.GetNamedNumber(timezoneKey, 0.0);
        m_verified = jsonObject.GetNamedBoolean(verifiedKey, false);

        for (IJsonValue jsonValue : jsonObject.GetNamedArray(educationKey, {}))
        {
            m_education.Append(SDKTemplate::School(jsonValue.GetObject()));
        }
    }

    hstring User::Stringify()
    {
        JsonObject jsonObject;

        jsonObject.SetNamedValue(idKey, JsonValue::CreateStringValue(m_id));

        // Treating a blank string as null
        if (m_phone.empty())
        {
            jsonObject.SetNamedValue(phoneKey, JsonValue::CreateNullValue());
        }
        else
        {
            jsonObject.SetNamedValue(phoneKey, JsonValue::CreateStringValue(m_phone));
        }

        jsonObject.SetNamedValue(nameKey, JsonValue::CreateStringValue(m_name));
        jsonObject.SetNamedValue(timezoneKey, JsonValue::CreateNumberValue(m_timezone));
        jsonObject.SetNamedValue(verifiedKey, JsonValue::CreateBooleanValue(m_verified));

        JsonArray jsonArray;
        for (SDKTemplate::School school : m_education)
        {
            jsonArray.Append(school.ToJsonObject());
        }

        // JsonObject can also be treated as an IMap.
        jsonObject.Insert(educationKey, jsonArray);

        return jsonObject.Stringify();
    }
}
