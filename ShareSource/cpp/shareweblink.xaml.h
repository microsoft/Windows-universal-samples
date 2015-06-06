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
// ShareWebLink.xaml.h
// Declaration of the ShareWebLink class
//

#pragma once

#include "pch.h"
#include "SharePage.h"
#include "ShareWebLink.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    public ref class ShareWebLink sealed
    {
    public:
        ShareWebLink();

    protected:
        virtual bool GetShareContent(Windows::ApplicationModel::DataTransfer::DataRequest^ request) override;

    private:
        Windows::Foundation::Uri^ ValidateAndGetUri(Platform::String^ uriString);
    };
}
