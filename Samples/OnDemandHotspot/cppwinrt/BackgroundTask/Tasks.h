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
#include "UpdateMetadataTask.g.h"
#include "ConnectTask.g.h"

namespace winrt::BackgroundTask::implementation
{
    struct UpdateMetadataTask : UpdateMetadataTaskT<UpdateMetadataTask>
    {
        void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance const& taskInstance);
    };

    struct ConnectTask : ConnectTaskT<ConnectTask>
    {
        void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance const& taskInstance);
        void DisplayToast(const winrt::hstring& ssid);
    };
}

namespace winrt::BackgroundTask::factory_implementation
{
    struct UpdateMetadataTask : UpdateMetadataTaskT<UpdateMetadataTask, implementation::UpdateMetadataTask>
    {
    };

    struct ConnectTask : ConnectTaskT<ConnectTask, implementation::ConnectTask>
    {
    };
}