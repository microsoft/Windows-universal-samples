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
#include "ViewModelLocator.h"

using namespace AdventureWorks;

using namespace Concurrency;
using namespace AdventureWorks_Shared;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

/// <summary> Create common trip store reference, set up view models.</summary>
ViewModelLocator::ViewModelLocator()
{
    this->store = ref new TripStore();
    this->modelSet = ref new Map<Platform::String^, ViewModelBase^>();
    this->modelSet->Insert(L"TripListViewModel", ref new AdventureWorks::TripListViewModel(store));
    this->modelSet->Insert(L"TripViewModel", ref new AdventureWorks::TripViewModel(store));
}

AdventureWorks::TripListViewModel^ ViewModelLocator::TripListViewModel::get()
{
    return (AdventureWorks::TripListViewModel^)this->modelSet->Lookup("TripListViewModel");
}

AdventureWorks::TripViewModel^ ViewModelLocator::TripViewModel::get()
{
    return (AdventureWorks::TripViewModel^)this->modelSet->Lookup("TripViewModel");
}