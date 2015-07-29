//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "PersistentState.h"
#include "DirectXSample.h"

using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace DirectX;

void PersistentState::Initialize(
    _In_ IPropertySet^ settingsValues,
    _In_ Platform::String^ key
    )
{
    m_settingsValues = settingsValues;
    m_keyName = key;
}

void PersistentState::SaveBool(Platform::String^ key, bool value)
{
    Platform::String^ fullKey = Platform::String::Concat(m_keyName, key);
    if (m_settingsValues->HasKey(fullKey))
    {
        m_settingsValues->Remove(fullKey);
    }
    m_settingsValues->Insert(fullKey, PropertyValue::CreateInt32(value ? 1 : 0));
}

void PersistentState::SaveInt32(Platform::String^ key, int value)
{
    Platform::String^ fullKey = Platform::String::Concat(m_keyName, key);
    if (m_settingsValues->HasKey(fullKey))
    {
        m_settingsValues->Remove(fullKey);
    }
    m_settingsValues->Insert(fullKey, PropertyValue::CreateInt32(value));
}

void PersistentState::SaveSingle(Platform::String^ key, float value)
{
    Platform::String^ fullKey = Platform::String::Concat(m_keyName, key);
    if (m_settingsValues->HasKey(fullKey))
    {
        m_settingsValues->Remove(fullKey);
    }
    m_settingsValues->Insert(fullKey, PropertyValue::CreateSingle(value));
}

void PersistentState::SaveXMFLOAT3(Platform::String^ key, DirectX::XMFLOAT3 value)
{
    Platform::String^ fullKey = Platform::String::Concat(m_keyName, key);
    if (m_settingsValues->HasKey(fullKey))
    {
        m_settingsValues->Remove(fullKey);
    }
    Platform::Array<float>^ array3 = ref new Platform::Array<float>(3);
    array3[0] = value.x;
    array3[1] = value.y;
    array3[2] = value.z;
    m_settingsValues->Insert(fullKey, PropertyValue::CreateSingleArray(array3));
}

void PersistentState::SaveString(Platform::String^ key, Platform::String^ string)
{
    Platform::String^ fullKey = Platform::String::Concat(m_keyName, key);
    if (m_settingsValues->HasKey(fullKey))
    {
        m_settingsValues->Remove(fullKey);
    }
    m_settingsValues->Insert(fullKey, PropertyValue::CreateString(string));
}

bool PersistentState::LoadBool(Platform::String^ key, bool defaultValue)
{
    Platform::String^ fullKey = Platform::String::Concat(m_keyName, key);
    if (m_settingsValues->HasKey(fullKey))
    {
        int state = safe_cast<IPropertyValue^>(m_settingsValues->Lookup(fullKey))->GetInt32();
        return state ? true : false;
    }
    return defaultValue;
}

int PersistentState::LoadInt32(Platform::String^ key, int defaultValue)
{
    Platform::String^ fullKey = Platform::String::Concat(m_keyName, key);
    if (m_settingsValues->HasKey(fullKey))
    {
        return safe_cast<IPropertyValue^>(m_settingsValues->Lookup(fullKey))->GetInt32();
    }
    return defaultValue;
}

float PersistentState::LoadSingle(Platform::String^ key, float defaultValue)
{
    Platform::String^ fullKey = Platform::String::Concat(m_keyName, key);
    if (m_settingsValues->HasKey(fullKey))
    {
        return safe_cast<IPropertyValue^>(m_settingsValues->Lookup(fullKey))->GetSingle();
    }
    return defaultValue;
}

XMFLOAT3 PersistentState::LoadXMFLOAT3(Platform::String^ key, DirectX::XMFLOAT3 defaultValue)
{
    XMFLOAT3 value;
    Platform::String^ fullKey = Platform::String::Concat(m_keyName, key);
    if (m_settingsValues->HasKey(fullKey))
    {
        Platform::Array<float>^ array3;
        safe_cast<IPropertyValue^>(m_settingsValues->Lookup(fullKey))->GetSingleArray(&array3);
        value.x = array3[0];
        value.y = array3[1];
        value.z = array3[2];
        return value;
    }
    return defaultValue;
}

Platform::String^ PersistentState::LoadString(Platform::String^ key, Platform::String^ defaultValue)
{
    Platform::String^ fullKey = Platform::String::Concat(m_keyName, key);
    if (m_settingsValues->HasKey(fullKey))
    {
        return safe_cast<IPropertyValue^>(m_settingsValues->Lookup(fullKey))->GetString();
    }
    return defaultValue;
}
