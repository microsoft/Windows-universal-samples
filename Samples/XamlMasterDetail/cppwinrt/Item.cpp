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
#include <winrt/MasterDetailApp.Data.h>
#include "Data.Item.g.h"

namespace winrt::MasterDetailApp::Data::implementation
{
    using namespace Windows::Foundation;

    struct Item : ItemT<Item>
    {
    public:
        Item(int id, DateTime dateCreated, hstring title, hstring text) :
            m_id{ id }, m_dateCreated{ dateCreated }, m_title{ std::move(title) }, m_text{ std::move(text) }
        {
        }

        int ItemId() { return m_id; }
        void ItemId(int value) { m_id = value; }

        Windows::Foundation::DateTime DateCreated() { return m_dateCreated; }
        void DateCreated(Windows::Foundation::DateTime value) { m_dateCreated = value; }

        hstring Title() { return m_title; }
        void Title(hstring value) { m_title = value; }

        hstring Text() { return m_text; }
        void Text(hstring value) { m_text = value; }

    private:
        int m_id;
        Windows::Foundation::DateTime m_dateCreated;
        hstring m_title;
        hstring m_text;
    };
}

namespace winrt::MasterDetailApp::Data::factory_implementation
{
    struct Item : ItemT<Item, implementation::Item>
    {
    };
}

#include "Data.Item.g.cpp"
