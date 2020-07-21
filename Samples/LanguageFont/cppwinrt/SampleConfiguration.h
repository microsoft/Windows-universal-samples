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
    template<typename T>
    void ApplyLanguageFont(T const& element, Windows::Globalization::Fonts::LanguageFont const& languageFont)
    {
        element.FontFamily({ languageFont.FontFamily() });
        element.FontWeight(languageFont.FontWeight());
        element.FontStyle(languageFont.FontStyle());
        element.FontStretch(languageFont.FontStretch());
    }
}
