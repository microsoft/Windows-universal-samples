// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

namespace WinRTComponent
{
    [Windows::Foundation::Metadata::AllowForWebAttribute]
    public ref class WinRTToastNotificationWrapper sealed
    {
    public:
        WinRTToastNotificationWrapper();

        void toastMessage(Platform::String^ message);
    };
}

