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

#include "App.g.h"
#include "ViewModel\ViewModelBase.h"
#include "ViewModel\TripListViewModel.h"
#include "ViewModel\TripViewModel.h"

namespace AdventureWorks
{
    [Windows::UI::Xaml::Data::Bindable]
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class ViewModelLocator sealed
    {
    public:
        ViewModelLocator();

        property TripListViewModel^ TripListViewModel
        {
            AdventureWorks::TripListViewModel^ get();
        }

        property TripViewModel^ TripViewModel
        {
            AdventureWorks::TripViewModel^ get();
        }

    private:
        Windows::Foundation::Collections::IMap<Platform::String^, ViewModelBase^>^ modelSet;
        AdventureWorks_Shared::TripStore^ store;

    };
}

