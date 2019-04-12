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
#include "App.h"

namespace winrt::SDKTemplate
{
    void App_LaunchCompleted(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const& e);
    void App_OnFileActivated(Windows::ApplicationModel::Activation::FileActivatedEventArgs const& e);
}
