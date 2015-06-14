//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Text;
using Windows.Foundation;
using Windows.Foundation.Metadata;

namespace Microsoft.SDKSamples.Kitchen
{
    // Explicitly declare IIDs to make it easier to author the proxy stub interface registration
    [Guid(0x699B1394, 0x3CEB, 0x4A14, 0xAE, 0x23, 0xEF, 0xEC, 0x51, 0x8B, 0x08, 0x8B)]
    public interface IBread
    {
        String Flavor { get; }
    }

    public sealed class Bread : IBread
    {
        public String Flavor 
        {
            get
            {
                return _flavor;
            }
        }

        internal Bread(String flavor)
        {
            _flavor = flavor;
        }

        private String _flavor;
    }
}
