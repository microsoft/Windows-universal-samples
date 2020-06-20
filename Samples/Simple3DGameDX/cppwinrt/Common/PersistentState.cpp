// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "PersistentState.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace DirectX;

void PersistentState::Initialize(
    _In_ IPropertySet settingsValues,
    _In_ winrt::hstring const& key
    )
{
    m_settingsValues = settingsValues;
    m_keyName = key;
}

void PersistentState::SaveBool(winrt::hstring const& key, bool value)
{
    m_settingsValues.Insert(m_keyName + key, winrt::box_value(value));
}

void PersistentState::SaveInt32(winrt::hstring const& key, int value)
{
    m_settingsValues.Insert(m_keyName + key, winrt::box_value(value));
}

void PersistentState::SaveSingle(winrt::hstring const& key, float value)
{
    m_settingsValues.Insert(m_keyName + key, winrt::box_value(value));
}

void PersistentState::SaveXMFLOAT3(winrt::hstring const& key, DirectX::XMFLOAT3 value)
{
    m_settingsValues.Insert(m_keyName + key, PropertyValue::CreateSingleArray({ value.x, value.y, value.z }));
}

void PersistentState::SaveString(winrt::hstring const& key, winrt::hstring const& string)
{
    m_settingsValues.Insert(m_keyName + key, winrt::box_value(string));
}

bool PersistentState::LoadBool(winrt::hstring const& key, bool defaultValue)
{
    return winrt::unbox_value_or(m_settingsValues.TryLookup(m_keyName + key), defaultValue);
}

int PersistentState::LoadInt32(winrt::hstring const& key, int defaultValue)
{
    return winrt::unbox_value_or(m_settingsValues.TryLookup(m_keyName + key), defaultValue);
}

float PersistentState::LoadSingle(winrt::hstring const& key, float defaultValue)
{
    return winrt::unbox_value_or(m_settingsValues.TryLookup(m_keyName + key), defaultValue);
}

XMFLOAT3 PersistentState::LoadXMFLOAT3(winrt::hstring const& key, DirectX::XMFLOAT3 defaultValue)
{
    auto propertyValue = winrt::unbox_value_or(m_settingsValues.TryLookup(m_keyName + key), winrt::Windows::Foundation::IPropertyValue{});
    if (propertyValue)
    {
        winrt::com_array<float> array3;
        propertyValue.GetSingleArray(array3);
        return { array3[0], array3[1], array3[2] };
    }
    return defaultValue;
}

winrt::hstring PersistentState::LoadString(winrt::hstring const& key, winrt::hstring const& defaultValue)
{
    return winrt::unbox_value_or<winrt::hstring>(m_settingsValues.TryLookup(m_keyName + key), defaultValue);
}