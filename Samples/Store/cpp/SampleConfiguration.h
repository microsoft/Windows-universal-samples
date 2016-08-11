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

#include <collection.h>

namespace SDKTemplate
{
    partial ref class ItemDetails;

    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };

    partial ref class MainPage
    {
    public:
        static property Platform::String^ FEATURE_NAME 
        {
            Platform::String^ get() 
            {  
                return "Store C++ Sample";
            }
        }

        static property Platform::Array<Scenario>^ scenarios 
        {
            Platform::Array<Scenario>^ get() 
            { 
                return scenariosInner; 
            }
        }

    private:
        static Platform::Array<Scenario>^ scenariosInner;
    };

    namespace Utils
    {
        int64_t DaysUntil(Windows::Foundation::DateTime endDate);

        Platform::Collections::Vector<ItemDetails^>^
            CreateProductListFromQueryResult(Windows::Services::Store::StoreProductQueryResult^ addOns, Platform::String^ description);

        void ReportExtendedError(Windows::Foundation::HResult extendedError);
    };

    // Helper functions for binding.
    public ref class BindingUtils sealed
    {
    public:
        static bool IsNonNull(Platform::Object^ o);
    };
}
