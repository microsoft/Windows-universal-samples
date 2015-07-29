// Copyright (c) Microsoft. All rights reserved.
#pragma once
#include "pch.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    public ref class Account sealed
    {
    public:
        property Platform::String^ Name
        {
            Platform::String^ get();
            void set(Platform::String^ value);
        };

        property Platform::String^ Email;
        property bool UsesPassport;

    private:
        Platform::String^ m_Name;
    };
}