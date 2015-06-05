//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include <wrl.h>
#include <winerror.h>
#include <windows.system.threading.h>

#include "Microsoft.SDKSamples.Kitchen.h"
#include "OvenServer.h"
#include "BreadServer.h"

using namespace Microsoft::WRL;

namespace ABI { namespace Microsoft { namespace SDKSamples { namespace Kitchen {

    IFACEMETHODIMP OvenFactory::ActivateInstance(
        _COM_Outptr_ IInspectable **ppOven)
    {
        *ppOven = nullptr;
        ComPtr<Oven> spOven;
        HRESULT hr = MakeAndInitialize<Oven>(&spOven);
        if (SUCCEEDED(hr))
        {
            hr = spOven.CopyTo(ppOven);
        }
        return hr;
    }

    IFACEMETHODIMP OvenFactory::CreateOven(
        _In_ Dimensions dimensions,
        _COM_Outptr_ IOven **ppOven)
    {
        ComPtr<Oven> spOven;
        HRESULT hr = MakeAndInitialize<Oven>(&spOven, dimensions);
        if (SUCCEEDED(hr))
        {
            *ppOven = spOven.Detach();
        }
        return hr;
    }

    IFACEMETHODIMP Oven::get_Volume(
        _Out_ double *pVolume)
    {
        *pVolume = _dims.Height * _dims.Width * _dims.Depth;
        return S_OK;
    }

    IFACEMETHODIMP Oven::ConfigurePreheatTemperature(
        _In_ OvenTemperature temperature)
    {
        HRESULT hr = S_OK;
        if (temperature >= OvenTemperature::Low && temperature <= OvenTemperature::High)
        {
            _temperature = temperature;
        }
        else
        {
            hr = E_INVALIDARG;
            ::RoOriginateErrorW(hr, 0, L"Temperature is out of range. The temperature should be low, medium, or high.");
        }
        return hr;
    }

    IFACEMETHODIMP Oven::BakeBread(
        _In_ HSTRING hstrFlavor)
    {
        HRESULT hr = S_OK;
        Windows::Foundation::TimeSpan preheatTime;

        // Determine how long we need to wait for the oven to get to temperature
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
            hr = E_INVALIDARG;
            ::RoOriginateErrorW(hr, 0, L"Temperature is out of range. The temperature should be low, medium, or high.");
        }

        // Servers can provide instances of other servers. The additional servers
        // may be provided just by interface, or metadata can specify
        // that the returned types are runtime classes.
        if (SUCCEEDED(hr))
        {
            ComPtr<::ABI::Windows::System::Threading::IThreadPoolTimerStatics> spThreadPoolTimerStatics;
            hr = Windows::Foundation::GetActivationFactory(Wrappers::HStringReference(RuntimeClass_Windows_System_Threading_ThreadPoolTimer).Get(), &spThreadPoolTimerStatics);
            if (SUCCEEDED(hr))
            {
                HSTRING hstrFlavorCopy;

                // Make a copy of the HSTRING to pass to the lambda
                hr = ::WindowsDuplicateString(hstrFlavor, &hstrFlavorCopy);
                if (SUCCEEDED(hr))
                {    
                    ComPtr<Oven> spThis(this);
                    ComPtr<::ABI::Windows::System::Threading::IThreadPoolTimer> spThreadPoolTimer;
                    auto spTimerCallback = Callback<::ABI::Windows::System::Threading::ITimerElapsedHandler>(
                        [spThis, hstrFlavorCopy](::ABI::Windows::System::Threading::IThreadPoolTimer *) -> HRESULT
                        {
                            ComPtr<Bread> spBread;
                            HRESULT hr = MakeAndInitialize<Bread>(&spBread, hstrFlavorCopy);
                            if (SUCCEEDED(hr))
                            {
                                hr = spThis->_evtBreadComplete.InvokeAll(spThis.Get(), spBread.Get());
                            }

                            // Free the string copy
                            ::WindowsDeleteString(hstrFlavorCopy);
                            return hr;
                        });

                    hr = spThreadPoolTimerStatics->CreateTimer(spTimerCallback.Get(), preheatTime, &spThreadPoolTimer);
                    if (FAILED(hr))
                    {
                        // If the timer callback failed to queue then free the string copy
                        ::WindowsDeleteString(hstrFlavorCopy);
                    }
                }
            }
        }
        return hr;
    }

    IFACEMETHODIMP Oven::add_BreadBaked(
        _In_ ::ABI::Windows::Foundation::ITypedEventHandler<Oven*, Bread*> *handler,
        _Out_ EventRegistrationToken *token)
    {
        return _evtBreadComplete.Add(handler, token);
    }

    IFACEMETHODIMP Oven::remove_BreadBaked(
        _In_ EventRegistrationToken token)
    {
        return _evtBreadComplete.Remove(token);
    }

    ActivatableClassWithFactory(Oven, OvenFactory)

} /* Kitchen */ } /* SDKSamples */ } /* Microsoft */ } /* ABI */
