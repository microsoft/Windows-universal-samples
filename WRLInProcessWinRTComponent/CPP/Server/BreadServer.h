//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include <wrl\implements.h>
#include <wrl\wrappers\corewrappers.h>

#include "Microsoft.SDKSamples.Kitchen.h"

namespace ABI { namespace Microsoft { namespace SDKSamples { namespace Kitchen {

    class Bread : public ::Microsoft::WRL::RuntimeClass<::ABI::Microsoft::SDKSamples::Kitchen::IBread>
    {
        InspectableClass(RuntimeClass_Microsoft_SDKSamples_Kitchen_Bread, TrustLevel::BaseTrust)

    public:
        // Non-projected method for setting up private state of Bread. This method is 
        // called by WRL::MakeAndInitialize
        HRESULT RuntimeClassInitialize(
            _In_ HSTRING hstrFlavor);

        // IBread::Flavor property
        IFACEMETHOD(get_Flavor)(
            _Out_ HSTRING *phstrFlavor);

    private:
        ::Microsoft::WRL::Wrappers::HString _hstrFlavor;
    };

} /* Kitchen */ } /* SDKSamples */ } /* Microsoft */ } /* ABI */
