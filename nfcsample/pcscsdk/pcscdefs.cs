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

namespace Pcsc
{
    /// <summary>
    /// PCSC storage standard enum (ss byte)
    /// </summary>
    public enum StorageStandard : byte
    {
        Unknown                         = 0x00,
        Iso14443AP1                     = 0x01,
        Iso14443AP2                     = 0x02,
        Iso14443AP3                     = 0x03,
        Iso14443ARfu                    = 0x04,

        Iso14443BP1                     = 0x05,
        Iso14443BP2                     = 0x06,
        Iso14443BP3                     = 0x07,
        Iso14443BRfu                    = 0x08,

        Iso15693P1                      = 0x09,
        Iso15693P2                      = 0x0A,
        Iso15693P3                      = 0x0B,
        Iso15693P4                      = 0x0C,

        Iso7816CI2C                     = 0x0D,
        Iso7816CEI2C                    = 0x0E,
        Iso7816C2WBP                    = 0x0F,
        Iso7816C3WBP                    = 0x10,

        FeliCa                          = 0x11,

        LowFreqCContactless             = 0x40,
    }

    /// <summary>
    /// PCSC card name enum (nn short)
    /// </summary>
    public enum CardName : ushort
    {
        Unknown                         = 0x0000,

        MifareStandard1K                = 0x0001,
        MifareStandard4K                = 0x0002,
        MifareUltralight                = 0x0003,
        SLE55R                          = 0x0004,
        SR176                           = 0x0006,
        SRIX4K                          = 0x0007,
        AT88RF020                       = 0x0008,
        AT88SC0204CRF                   = 0x0009,
        AT88SC0808CRF                   = 0x000A,
        AT88SC1616CRF                   = 0x000B,
        AT88SC3216CRF                   = 0x000C,
        AT88SC6416CRF                   = 0x000D,
        SRF55V10P                       = 0x000E,
        SRF55V02P                       = 0x000F,
        SRF55V10S                       = 0x0010,
        SRF55V02S                       = 0x0011,
        TAG_IT                          = 0x0012,
        LRI512                          = 0x0013,
        ICODESLI                        = 0x0014,
        TEMPSENS                        = 0x0015,
        ICODE1                          = 0x0016,
        PicoPass2K                      = 0x0017,
        PicoPass2KS                     = 0x0018,
        PicoPass16K                     = 0x0019,
        PicoPass16Ks                    = 0x001A,
        PicoPass16K8x2                  = 0x001B,
        PicoPass16KS8x2                 = 0x001C,
        PicoPass32KS16p16               = 0x001D,
        PicoPass32KS16p8x2              = 0x001E,
        PicoPass32KS8x2p16              = 0x001F,
        PicoPass32KS8x2p8x2             = 0x0020,
        LRI64                           = 0x0021,
        ICODEUID                        = 0x0022,
        ICODEEPC                        = 0x0023,
        LRI12                           = 0x0024,
        LRI128                          = 0x0025,
        MifareMini                      = 0x0026,
        SLE66R01P                       = 0x0027,
        SLE66RxxP                       = 0x0028,
        SLE66RxxS                       = 0x0029,
        SLE55RxxE                       = 0x002A,
        SRF55V01P                       = 0x002B,
        SRF66V10ST                      = 0x002C,
        SRF66V10IT                      = 0x002D,
        SRF66V01ST                      = 0x002E,
        JewelTag                        = 0x002F,
        TopazTag                        = 0x0030,
        AT88SC0104CRF                   = 0x0031,
        AT88SC0404CRF                   = 0x0032,
        AT88RF01C                       = 0x0033,
        AT88RF04C                       = 0x0034,
        iCodeSL2                        = 0x0035,
        MifarePlusSL1_2K                = 0x0036,
        MifarePlusSL1_4K                = 0x0037,
        MifarePlusSL2_2K                = 0x0038,
        MifarePlusSL2_4K                = 0x0039,
        MifareUltralightC               = 0x003A,
        FeliCa                          = 0x003B,
        MelexisSensorTagMLX90129        = 0x003C,
        MifareUltralightEV1             = 0x003D,

        CardNameMaxValue                = MifareUltralightEV1,
    }

    /// <summary>
    /// Enumeration of possible instructions 
    /// </summary>
    public enum Ins : byte
    {
        GetData = 0xCA,
        LoadKeys = 0x82,
        GeneralAuthenticate = 0x86,
        Verify = 0x20,
        ReadBinary = 0xB0,
        UpdateBinary = 0xD6,
    }
}
