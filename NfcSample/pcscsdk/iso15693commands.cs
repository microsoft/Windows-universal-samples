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

using System;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Storage.Streams;

namespace Iso15693
{
    /// <summary>
    /// Iso15693 read command
    /// </summary>
    public class Read : Pcsc.ReadBinary
    {
        public Read(ushort blockNumber)
            : base((ushort)((blockNumber & 0xff) << 8), 4)
        {
        }
    }
    /// <summary>
    /// Iso15693 write command
    /// </summary>
    public class Write : Pcsc.UpdateBinary
    {
        public Write(ushort blockNumber, byte[] dataToWrite)
            : base((ushort)((blockNumber & 0xff) << 8), dataToWrite)
        {
        }
    }
    /// <summary>
    /// Iso15693 GetUid command
    /// </summary>
    public class GetUid : Pcsc.GetUid
    {
        public GetUid()
            : base()
        {
        }
    }
    /// <summary>
    /// Iso15693 GetHistoricalBytes command
    /// </summary>
    public class GetHistoricalBytes : Pcsc.GetHistoricalBytes
    {
        public GetHistoricalBytes()
            : base()
        {
        }
    }
    /// <summary>
    /// Iso15693 response APDU
    /// </summary>
    public class ApduResponse : Pcsc.ApduResponse
    {
        public ApduResponse()
            : base()
        {
        }
    }
}
