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
#include "GroupInfoList.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct GroupInfoList : GroupInfoListT<GroupInfoList>
    {
        GroupInfoList() = default;

        hstring Key() { return key; }
        void Key(hstring const& value) { key = value; }
        auto First() { return vector.First(); }
        auto GetAt(uint32_t index) { return vector.GetAt(index); }
        auto Size() { return vector.Size(); }
        auto GetView() { return vector.GetView(); }
        auto IndexOf(Windows::Foundation::IInspectable const& value, uint32_t& index) { return vector.IndexOf(value, index); }
        auto SetAt(uint32_t index, Windows::Foundation::IInspectable const& value) { return vector.SetAt(index, value); }
        auto InsertAt(uint32_t index, Windows::Foundation::IInspectable const& value) { return vector.InsertAt(index, value); }
        auto RemoveAt(uint32_t index) { return vector.RemoveAt(index); }
        auto Append(Windows::Foundation::IInspectable const& value) { return vector.Append(value); }
        auto RemoveAtEnd() { return vector.RemoveAtEnd(); }
        auto Clear() { return vector.Clear(); }
        auto GetMany(uint32_t startIndex, array_view<Windows::Foundation::IInspectable> items) { return vector.GetMany(startIndex, items); }
        auto ReplaceAll(array_view<Windows::Foundation::IInspectable const> items) { return vector.ReplaceAll(items); }

    private:
        hstring key;
        Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable> vector = single_threaded_vector<Windows::Foundation::IInspectable>();
    };
}
namespace winrt::SDKTemplate::factory_implementation
{
    struct GroupInfoList : GroupInfoListT<GroupInfoList, implementation::GroupInfoList>
    {
    };
}