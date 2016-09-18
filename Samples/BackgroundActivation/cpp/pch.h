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

#include <vccorlib.h>
#include <collection.h>
#include <ppltasks.h>

#include "App.g.h"

namespace SDKTemplate
{
    partial ref class App
    {
        /// <summary>
        /// Override the Application OnBackgroundActivated method to handle background activation in
        /// the main process. This entry point is used when BackgroundTaskBuilder TaskEntryPoint is
        /// not set during background task registration.
        /// </summary>
        /// <param name="args"></param>
    protected:
        virtual void OnBackgroundActivated(Windows::ApplicationModel::Activation::BackgroundActivatedEventArgs^ args) override;
    };
}
#include "App.xaml.h"