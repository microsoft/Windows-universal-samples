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

//
// ShareApplicationLink.xaml.h
// Declaration of the ShareApplicationLink class
//

#pragma once

#include "pch.h"
#include "SharePage.h"
#include "ShareApplicationLink.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    public ref class ShareApplicationLink sealed
    {
    public:
        ShareApplicationLink();

    protected:
        virtual bool GetShareContent(Windows::ApplicationModel::DataTransfer::DataRequest^ request) override;

    private:
        Windows::UI::Color GetLogoBackgroundColor();
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class NegatingBoolConverter sealed : Windows::UI::Xaml::Data::IValueConverter
    {
    public:
        virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language)
        {
            bool v = safe_cast<bool>(value);
            return !v;
        }

        virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language)
        {
            bool v = safe_cast<bool>(value);
            return !v;
        }
    };
}
