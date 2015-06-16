//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include "pch.h"

namespace Microsoft { namespace SDKSamples { namespace Kitchen {

    double Oven::Volume::get()
    {
        return _dims.Width * _dims.Height * _dims.Depth;
    }

    void Oven::ConfigurePreheatTemperature(OvenTemperature temperature)
    {
        if (temperature >= OvenTemperature::Low && temperature <= OvenTemperature::High)
        {
            _temperature = temperature;
        }
        else
        {
            throw ref new Platform::InvalidArgumentException(L"Temperature is out of range. The temperature should be low, medium, or high.");
        }
    }

    void Oven::BakeBread(Platform::String^ flavor)
    {
        Windows::Foundation::TimeSpan preheatTime;

        // Determine how long to wait for the oven to get to temperature
        switch (_temperature)
        {
        case OvenTemperature::Low:
            // Set the preheat time to 100ms
            preheatTime.Duration = 1000;
            break;

        case OvenTemperature::Medium:
            // Set the preheat time to 200ms
            preheatTime.Duration = 2000;
            break;

        case OvenTemperature::High:
            // Set the preheat time to 300ms
            preheatTime.Duration = 3000;
            break;

        default:
            throw ref new Platform::InvalidArgumentException(L"Temperature is out of range. The temperature should be low, medium, or high.");
        }

        // Set up a threadpool work item to bake the bread and notify listeners after the oven has preheated
        Windows::System::Threading::ThreadPoolTimer::CreateTimer(ref new Windows::System::Threading::TimerElapsedHandler(
            [this, flavor](Windows::System::Threading::ThreadPoolTimer^ timer) -> void
            {
                Bread^ bread = ref new Bread(flavor);
                BreadBaked(this, bread);
            }),
            preheatTime);
    }
} /* Kitchen */ } /* SDKSamples */ } /* Microsoft */
