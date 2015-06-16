// Copyright (c) Microsoft. All rights reserved.

#pragma once 
#include "pch.h"

namespace xBindSampleCX
{
    ref class Scenario;

    partial ref class MainPage
    {
    public:
        property Platform::String^ FEATURE_NAME
        {
            Platform::String^ get()
            {
                return "xBind Sample in C++/CX";
            }
        }
    internal:
        static property Windows::Foundation::Collections::IVector<Scenario^>^ scenarios
        {
            Windows::Foundation::Collections::IVector<Scenario^>^ get()
            {
                return scenariosInner;
            }
        }

    private:
        static Platform::Collections::Vector<Scenario^>^ scenariosInner;
    };

    public ref class Scenario sealed
    {
    public:
        property  Platform::String^ Title;
        property  Platform::String^ Description;
        property  Platform::String^ ClassName;

        Scenario(Platform::String^ title, Platform::String^ description, Platform::String^ className);
    };
}
