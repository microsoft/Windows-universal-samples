// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

namespace WinRTComponent
{
    
    public delegate void NotifyAppHandler(int number);
    
    [Windows::Foundation::Metadata::AllowForWebAttribute]
    public ref class WinRTSharedObject sealed
    {
    public:
        WinRTSharedObject();

        event NotifyAppHandler^ NotifyAppOfPrimes;
        void CheckPrimesInRange();
        bool IsPrime(int number);

        void setStartNumber(int num);
        void setEndNumber(int num);

    private:
        int startNumber;
        int endNumber;
    };
}
