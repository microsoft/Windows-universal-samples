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

namespace MifareUltralight
{
    /// <summary>
    /// Mifare UL Read commad when sent to the card the card is expected to return 16 byte of 4 Mifare
    /// pages starting at address addr.
    /// </summary>
    public class Read : Pcsc.ReadBinary
    {
        public Read(byte pageAddress)
            : base(pageAddress, 16)
        {
        }
    }
    /// <summary>
    /// Mifare UL Write commad when sent to the card, writes 4 bytes (1 Mifare block) at given
    /// block address addr.
    /// </summary>
    public class Write : Pcsc.UpdateBinary
    {
        public byte[] Data
        {
            set { base.CommandData = ((value.Length != 4) ? ResizeArray(value, 4) : value); }
            get { return base.CommandData; }
        }
        private static byte[] ResizeArray(byte[] data, int size)
        {
            Array.Resize<byte>(ref data, size);
            return data;
        }
        public Write(byte address, ref byte[] data)
            : base(address, ((data.Length != 4) ? ResizeArray(data, 4) : data))
        {
        }
    }
    /// <summary>
    /// Mifare UL GetUid command
    /// </summary>
    public class GetUid : Pcsc.GetUid
    {
        public GetUid()
            : base()
        {
        }
    }
    /// <summary>
    /// Mifare UL GetHistoricalBytes command
    /// </summary>
    public class GetHistoricalBytes : Pcsc.GetHistoricalBytes
    {
        public GetHistoricalBytes()
            : base()
        {
        }
    }
    /// <summary>
    /// Mifare response APDU
    /// </summary>
    public class ApduResponse : Pcsc.ApduResponse
    {
        public ApduResponse()
            : base()
        {
        }
    }
}
