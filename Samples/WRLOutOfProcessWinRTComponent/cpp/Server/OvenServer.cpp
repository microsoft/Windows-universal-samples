//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include <wrl.h>
#include <winerror.h>
#include <Windows.Foundation.h>
#include <wrl\implements.h>
#include <wrl\event.h>
#include <windows.system.threading.h>

#include "Microsoft.SDKSamples.Kitchen.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Microsoft::SDKSamples::Kitchen;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::System::Threading;

namespace ABI { namespace Microsoft { namespace SDKSamples { namespace Kitchen {

                class Bread WrlFinal : public RuntimeClass<IBread>
                {
                    InspectableClass(RuntimeClass_Microsoft_SDKSamples_Kitchen_Bread, TrustLevel::BaseTrust);
                public:
                    HRESULT RuntimeClassInitialize(HSTRING flavor)
                    {
                        return m_flavor.Set(flavor);
                    }

                    // IBread
                    STDMETHODIMP get_Flavor(_COM_Outptr_ HSTRING* value) override
                    {
                        return m_flavor.CopyTo(value);
                    }
                    HString m_flavor;
                };

                class Oven WrlFinal : public RuntimeClass<IOven, IAppliance>
                {
                    InspectableClass(RuntimeClass_Microsoft_SDKSamples_Kitchen_Oven, TrustLevel::BaseTrust);

                public:
                    HRESULT RuntimeClassInitialize()
                    {
                        return S_OK;
                    }

                    HRESULT RuntimeClassInitialize(_In_ Dimensions dimensions)
                    {
                        _dims = dimensions;
                        return S_OK;
                    }

                    // IAppliance
                    STDMETHODIMP get_Volume(_Out_ double* value) override;

                    // IOven
                    STDMETHODIMP ConfigurePreheatTemperature(_In_ OvenTemperature temperature) override;
                    STDMETHODIMP BakeBread(_In_ HSTRING flavor) override;
                    STDMETHODIMP add_BreadBaked(_In_ ITypedEventHandler<Oven*, BreadBakedEventArgs*>* handler, _Out_ EventRegistrationToken *token) override;
                    STDMETHODIMP remove_BreadBaked(_In_ EventRegistrationToken token) override;

                private:
                    AgileEventSource<ITypedEventHandler<Oven*, BreadBakedEventArgs*>> _breadBaked;
                    Dimensions _dims = { 1.0, 1.0, 1.0 };
                    OvenTemperature _temperature = OvenTemperature::Medium;
                };

                class BreadBakedEventArgs WrlFinal : public RuntimeClass<IBreakBakedEventArgs>
                {
                    InspectableClass(RuntimeClass_Microsoft_SDKSamples_Kitchen_BreadBakedEventArgs, TrustLevel::BaseTrust)

                public:
                    HRESULT RuntimeClassInitialize(_In_ Bread* bread)
                    {
                        m_bread = bread;
                        return S_OK;
                    }

                    // IBreakBakedEventArgs
                    STDMETHODIMP get_Bread(_COM_Outptr_ IBread** value) override
                    {
                        return m_bread.CopyTo(value);
                    }

                private:
                    ComPtr<Bread> m_bread;
                };

                class OvenFactory WrlFinal : public ActivationFactory<IOvenFactory>
                {
                public:
                    // IActivationFactory
                    STDMETHODIMP ActivateInstance(_COM_Outptr_ IInspectable** oven) override
                    {
                        return MakeAndInitialize<Oven>(oven);
                    }

                    // IOvenFactory
                    STDMETHODIMP CreateOven(_In_ Dimensions dimensions, _COM_Outptr_ IOven** oven) override
                    {
                        return MakeAndInitialize<Oven>(oven, dimensions);
                    }
                };

                STDMETHODIMP Oven::get_Volume(_Out_ double* value)
                {
                    *value = _dims.Height * _dims.Width * _dims.Depth;
                    return S_OK;
                }

                STDMETHODIMP Oven::ConfigurePreheatTemperature(_In_ OvenTemperature temperature)
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

                STDMETHODIMP Oven::BakeBread(_In_ HSTRING flavor)
                {
                    HRESULT hr = S_OK;
                    Windows::Foundation::TimeSpan preheatTime;

                    // Determine how long we need to wait for the oven to get to temperature
                    switch (_temperature)
                    {
                    case OvenTemperature::Low:
                        preheatTime.Duration = 1000000;    // preheat time to 100ms
                        break;

                    case OvenTemperature::Medium:
                        preheatTime.Duration = 2000000;    // preheat time to 200ms
                        break;

                    case OvenTemperature::High:
                        preheatTime.Duration = 3000000;    // preheat time to 300ms
                        break;

                    default:
                        hr = E_INVALIDARG;
                        ::RoOriginateErrorW(hr, 0, L"Temperature is out of range. The temperature should be low, medium, or high.");
                    }

                    if (SUCCEEDED(hr))
                    {
                        ComPtr<IThreadPoolTimerStatics> threadPoolTimer;
                        hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_System_Threading_ThreadPoolTimer).Get(), &threadPoolTimer);
                        if (SUCCEEDED(hr))
                        {
                            // Oven makes bread
                            ComPtr<Bread> bread;
                            hr = MakeAndInitialize<Bread>(&bread, flavor);
                            if (SUCCEEDED(hr))
                            {
                                ComPtr<BreadBakedEventArgs> eventArg;
                                hr = MakeAndInitialize<BreadBakedEventArgs>(&eventArg, bread.Get());
                                if (SUCCEEDED(hr))
                                {
                                    ComPtr<Oven> thisRef(this);
                                    auto spTimerCallback = Callback<ITimerElapsedHandler>([thisRef, eventArg](IThreadPoolTimer*) -> HRESULT
                                    {
                                        return thisRef->_breadBaked.InvokeAll(thisRef.Get(), eventArg.Get());
                                    });

                                    hr = spTimerCallback ? S_OK : E_OUTOFMEMORY;
                                    if (SUCCEEDED(hr))
                                    {
                                        ComPtr<IThreadPoolTimer> timer; // unused but required out param
                                        hr = threadPoolTimer->CreateTimer(spTimerCallback.Get(), preheatTime, &timer);
                                    }
                                }
                            }
                        }
                    }
                    return hr;
                }

                STDMETHODIMP Oven::add_BreadBaked(_In_ ITypedEventHandler<Oven*, BreadBakedEventArgs*>* handler, _Out_ EventRegistrationToken* token)
                {
                    return _breadBaked.Add(handler, token);
                }

                STDMETHODIMP Oven::remove_BreadBaked(_In_ EventRegistrationToken token)
                {
                    return _breadBaked.Remove(token);
                }

                ActivatableClassWithFactory(Oven, OvenFactory)

} /* Kitchen */ } /* SDKSamples */ } /* Microsoft */ } /* ABI */
