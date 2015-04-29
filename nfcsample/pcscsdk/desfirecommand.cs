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

namespace Desfire
{
    /// <summary>
    /// Class DesfireCommand extends the Iso7816.ApduCommand and provides
    /// mappings to Iso7816 command fields
    /// </summary>
    public class DesfireCommand : Iso7816.ApduCommand
    {
        public byte Command
        {
            set { base.INS = value; }
            get { return base.INS; }
        }
        public byte[] Data
        {
            set { base.CommandData = value; }
            get { return base.CommandData; }
        }
        public enum CommandType : byte
        {
            GetVersion = 0x60,
            GetAdditionalFrame = 0xAF,
            SelectApplication = 0x5A,
            ReadData = 0xBD,
            ReadRecord = 0xBB
        };

        public DesfireCommand()
            : base((byte)Iso7816.Cla.ProprietaryCla9x, 0, 0, 0, null, 0)
        {
            ApduResponseType = typeof(Desfire.DesfireResponse);
        }
        public DesfireCommand(CommandType cmd, byte[] data)
            : base((byte)Iso7816.Cla.ProprietaryCla9x, (byte)cmd, 0x00, 0x00, data, 0x00)
        {
            ApduResponseType = typeof(Desfire.DesfireResponse);
        }
    }
    /// <summary>
    /// Class DesfireResponse extends the Iso7816.ApduResponse.
    /// </summary>
    public class DesfireResponse : Iso7816.ApduResponse
    {
        public DesfireResponse()
            : base()
        { }
        public override string SWTranslation
        {
            get
            {
                if (SW1 != 0x91)
                {
                    return "Unknown";
                }
                switch (SW2)
                {
                    case 0x00:
                        return "Success";

                    case 0xAF:
                        return "Additional frames expected";

                    default:
                        return "Unknown";
                }
            }
        }
        public override bool Succeeded
        {
            get
            {
                return SW == 0x9100;
            }
        }
        public bool SubsequentFrame
        {
            get
            {
                return SW == 0x91AF;
            }
        }
        public bool BoundaryError
        {
            get
            {
                return SW == 0x91BE;
            }
        }
    }
}
