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
// ShareCustomData.xaml.h
// Declaration of the ShareCustomData class
//

#pragma once

#include "pch.h"
#include "SharePage.h"
#include "ShareCustomData.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class ShareCustomData sealed
    {
    public:
        ShareCustomData();

    protected:
        virtual bool GetShareContent(Windows::ApplicationModel::DataTransfer::DataRequest^ request) override;
    };
}
