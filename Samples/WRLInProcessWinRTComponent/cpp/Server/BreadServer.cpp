//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include "Microsoft.SDKSamples.Kitchen.h"
#include "BreadServer.h"

namespace ABI { namespace Microsoft { namespace SDKSamples { namespace Kitchen {

    HRESULT Bread::RuntimeClassInitialize(
        _In_ HSTRING flavor)
    {
        return _hstrFlavor.Set(flavor);
    }

    IFACEMETHODIMP Bread::get_Flavor(
        _Out_ HSTRING *flavor)
    {
        return ::WindowsDuplicateString(_hstrFlavor.Get(), flavor);
    }

} /* Kitchen */ } /* SDKSamples */ } /* Microsoft */ } /* ABI */
