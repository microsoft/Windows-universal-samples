//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include <Windows.Foundation.h>
#include <wrl\implements.h>
#include <wrl\event.h>

#include "Microsoft.SDKSamples.Kitchen.h"

namespace ABI { namespace Microsoft { namespace SDKSamples { namespace Kitchen {

    class OvenFactory : public ::Microsoft::WRL::ActivationFactory<
        ::ABI::Microsoft::SDKSamples::Kitchen::IOvenFactory>
    {
    public:
        // IActivationFactory::ActivateInstance
        IFACEMETHOD(ActivateInstance)(
            _COM_Outptr_ IInspectable **ppOven);

        // IOvenFactory::CreateOven
        IFACEMETHOD(CreateOven)(
            _In_ Dimensions dimensions,
            _COM_Outptr_ IOven **ppOven);
    };

    class Oven : public ::Microsoft::WRL::RuntimeClass<
        ::ABI::Microsoft::SDKSamples::Kitchen::IOven,
        ::ABI::Microsoft::SDKSamples::Kitchen::IAppliance>
    {
        InspectableClass(RuntimeClass_Microsoft_SDKSamples_Kitchen_Oven, TrustLevel::BaseTrust);

    public:
        Oven() :
          _dims(),
          _temperature(OvenTemperature::Medium)
        {
            _dims.Depth = 1.0;
            _dims.Height = 1.0;
            _dims.Width = 1.0;
        }

        Oven(_In_ Dimensions dimensions) :
          _dims(dimensions),
          _temperature(OvenTemperature::Medium)
        {
        }

    public:
        // Non-projected method for setting up private state of Oven. This method is 
        // called by WRL::MakeAndInitialize
        HRESULT RuntimeClassInitialize()
        {
            _dims.Depth  = 1.0;
            _dims.Height = 1.0;
            _dims.Width  = 1.0;
            _temperature = OvenTemperature::Medium;

            return S_OK;
        }

        // Non-projected method for setting up private state of Oven. This method is 
        // called by WRL::MakeAndInitialize
        HRESULT RuntimeClassInitialize(
            _In_ Dimensions dimensions)
        {
            _dims = dimensions;
            _temperature = OvenTemperature::Medium;

            return S_OK;
        }

        // IAppliance::get_Volume
        IFACEMETHOD(get_Volume)(
            _Out_ double *pVolume);

        // IOven::ConfigurePreheatTemperature
        IFACEMETHOD(ConfigurePreheatTemperature)(
            _In_ OvenTemperature temperature);

        // IOven::IBread
        IFACEMETHOD(BakeBread)(
            _In_  HSTRING hstrFlavor);

        // IOven::add_BreadBaked
        IFACEMETHOD(add_BreadBaked)(
            _In_ ::ABI::Windows::Foundation::ITypedEventHandler<Oven*, Bread*> *clickHandler,
            _Out_ EventRegistrationToken *token);

        // IOven::remove_BreadBaked
        IFACEMETHOD(remove_BreadBaked)(
            _In_ EventRegistrationToken token);

    private:
        ::Microsoft::WRL::AgileEventSource<::ABI::Windows::Foundation::ITypedEventHandler<Oven*, Bread*>, ::Microsoft::WRL::InvokeModeOptions<::Microsoft::WRL::StopOnFirstError> > _evtBreadComplete;
        Dimensions _dims;
        OvenTemperature _temperature;
    };

} /* Kitchen */ } /* SDKSamples */ } /* Microsoft */ } /* ABI */
