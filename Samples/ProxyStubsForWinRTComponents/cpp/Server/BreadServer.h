//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#pragma once

namespace Microsoft { namespace SDKSamples { namespace Kitchen {

    // Explicitly declare IIDs to make it easier to author the proxy stub interface registration
    [uuid(699B1394-3CEB-4A14-AE23-EFEC518B088B)]
    public interface class IBread
    {
        property Platform::String^ Flavor
        {
            Platform::String^ get();
        }
    };

    public ref class Bread sealed : public [Windows::Foundation::Metadata::Default] IBread
    {
    internal:
        Bread(Platform::String^ flavor);

    public:
        virtual property Platform::String^ Flavor
        {
            // Read only property
            virtual Platform::String^ get();
        }

    private:
        Platform::String^ _flavor;
    };

} /* Kitchen */ } /* SDKSamples */ } /* Microsoft */
