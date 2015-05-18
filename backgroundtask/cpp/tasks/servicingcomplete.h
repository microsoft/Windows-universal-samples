//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include "pch.h"

using namespace Windows::ApplicationModel::Background;

namespace Tasks
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class ServicingComplete sealed : public IBackgroundTask
    {

    public:
        ServicingComplete();

        virtual void Run(IBackgroundTaskInstance^ taskInstance);
        void OnCanceled(IBackgroundTaskInstance^ taskInstance, BackgroundTaskCancellationReason reason);

    private:
        ~ServicingComplete();

        volatile bool CancelRequested;
    };
}
