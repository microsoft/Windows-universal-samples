#include "stdafx.h"
#include <windows.h>

#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex> 

namespace RpcServer
{
    class Metering
    {
    public:
        Metering(__int64 period);
        void SetSamplePeriod(__int64 period);
        __int64 GetMeteringData() const;
        void MeteringWorker();
        void WaitForMeteringData() const;
        void StartMetering(__int64 samplePeriod, __int64 context);
        void StopMetering();
        ~Metering();

    private:
        volatile __int64 _data;
        volatile __int64 samplePeriod;
        PTP_TIMER tpTimer = nullptr;
        HANDLE event;
        volatile bool stopMeteringRequested = false;
    };
}
