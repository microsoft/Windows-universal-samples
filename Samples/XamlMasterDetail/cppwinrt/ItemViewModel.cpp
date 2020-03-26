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
#include "ViewModels.ItemViewModel.g.h"

namespace winrt::MasterDetailApp::ViewModels::implementation
{
    struct ItemViewModel : ItemViewModelT<ItemViewModel>
    {
        ItemViewModel(MasterDetailApp::Data::Item const& item) :
            m_id{ item.ItemId() }, m_title{ item.Title() },
            m_text{ item.Text() }, m_dateCreated{ item.DateCreated() }
        {
        }

        static MasterDetailApp::ViewModels::ItemViewModel FromItem(MasterDetailApp::Data::Item const& item)
        {
            return make<ItemViewModel>(item);
        }

        int ItemId() { return m_id; }
        hstring Title() { return m_title; }
        hstring Text() { return m_text; }
        Windows::Foundation::DateTime DateCreated() { return m_dateCreated; }

        hstring DateCreatedHourMinute()
        {
            auto formatter = Windows::Globalization::DateTimeFormatting::DateTimeFormatter(L"hour minute");
            return formatter.Format(m_dateCreated);
        }

    private:
        int const m_id;
        hstring const m_title;
        hstring m_text;
        Windows::Foundation::DateTime const m_dateCreated;
    };
}

namespace winrt::MasterDetailApp::ViewModels::factory_implementation
{
    struct ItemViewModel : ItemViewModelT<ItemViewModel, implementation::ItemViewModel>
    {
    };
}

#include "ViewModels.ItemViewModel.g.cpp"
