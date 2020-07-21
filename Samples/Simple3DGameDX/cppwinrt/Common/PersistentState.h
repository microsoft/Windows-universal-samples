// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

// A simple helper class that provides support for saving and loading various
// data types to an IPropertySet. Used by DirectX SDK samples to implement
// process lifetime management (PLM).

class PersistentState
{
public:
    void Initialize(
        _In_ winrt::Windows::Foundation::Collections::IPropertySet settingsValues,
        _In_ winrt::hstring const& key
        );

    void SaveBool(winrt::hstring const& key, bool value);
    void SaveInt32(winrt::hstring const& key, int value);
    void SaveSingle(winrt::hstring const& key, float value);
    void SaveXMFLOAT3(winrt::hstring const& key, DirectX::XMFLOAT3 value);
    void SaveString(winrt::hstring const& key, winrt::hstring const& string);

    bool LoadBool(winrt::hstring const& key, bool defaultValue);
    int  LoadInt32(winrt::hstring const& key, int defaultValue);
    float LoadSingle(winrt::hstring const& key, float defaultValue);
    DirectX::XMFLOAT3 LoadXMFLOAT3(winrt::hstring const& key, DirectX::XMFLOAT3 defaultValue);
    winrt::hstring LoadString(winrt::hstring const& key, winrt::hstring const& defaultValue);

private:
    winrt::hstring m_keyName;
    winrt::Windows::Foundation::Collections::IPropertySet m_settingsValues;
};
