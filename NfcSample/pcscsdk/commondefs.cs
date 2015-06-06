//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

namespace Pcsc.Common
{
    /// <summary>
    /// ICC Device class
    /// </summary>
    public enum DeviceClass : byte
    {
        Unknown             = 0x00,
        StorageClass        = 0x01,  // for PCSC class, there will be subcategory to identify the physical icc
        Iso14443P4          = 0x02,
        MifareDesfire       = 0x03,
    }
}
