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
#include "pch.h"

namespace winrt::SDKTemplate
{
    bool TryParseInt32(wchar_t const* text, int32_t& value);

    // RAII type for ensuring an object is closed.
    struct ensure_close
    {
        Windows::Foundation::IClosable m_closable;

        ensure_close(Windows::Foundation::IClosable closable) : m_closable(std::move(closable)) { }
        ~ensure_close() { if (m_closable) m_closable.Close(); }

        ensure_close(const ensure_close&) = delete;
        ensure_close& operator=(const ensure_close&) = delete;

        ensure_close(ensure_close&& other) = default;
        ensure_close& operator=(ensure_close&& other) = default;
    };


}
