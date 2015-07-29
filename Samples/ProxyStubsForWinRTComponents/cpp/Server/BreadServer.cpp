//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include "pch.h"

namespace Microsoft { namespace SDKSamples { namespace Kitchen {

    Bread::Bread(Platform::String^ flavor) :
        _flavor(flavor)
    {
    }

    Platform::String^ Bread::Flavor::get()
    {
        return _flavor;
    }

} /* Kitchen */ } /* SDKSamples */ } /* Microsoft */
