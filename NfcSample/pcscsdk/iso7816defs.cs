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

namespace Iso7816
{
    /// <summary>
    /// Enumeration of possible ISO 7816 Command 
    /// </summary>
    public enum Cla : byte
    {
        CompliantCmd0x          = 0x00,
        AppCompliantCmdAx       = 0xA0,
        ProprietaryCla8x        = 0x80,
        ProprietaryCla9x        = 0x90,
        ReservedForPts          = 0xFF,           // Protocol Type Selelction
    }
    /// <summary>
    /// Enumeration of lower nibbile of CLA 
    /// </summary>
    public enum ClaXx : byte
    {
        NoSmOrNoSmIndication    = 0x00,
        ProprietarySmFormat     = 0x01,
        SecureMessageNoHeaderSM = 0x10,
        SecureMessage1p6        = 0x11,
    }
    /// <summary>
    /// Enumeration of possible instructions 
    /// </summary>
    public enum Ins : byte
    {
        EraseBinary             = 0x0E,
        Verify                  = 0x20,
        ManageChannel           = 0x70,
        ExternalAuthenticate    = 0x82,
        GetChallenge            = 0x84,
        InternalAuthenticate    = 0x88,
        SelectFile              = 0xA4,
        ReadBinary              = 0xB0,
        ReadRecords             = 0xB2,
        GetResponse             = 0xC0,
        Envelope                = 0xC2,
        GetData                 = 0xCA,
        WriteBinary             = 0xD0,
        WriteRecord             = 0xD2,
        UpdateBinary            = 0xD6,
        PutData                 = 0xDA,
        UpdateData              = 0xDC,
        AppendRecord            = 0xE2,
    }
}
