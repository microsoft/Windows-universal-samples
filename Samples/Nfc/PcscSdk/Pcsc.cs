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

using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Storage.Streams;

namespace Pcsc
{
    /// <summary>
    /// PCSC GetData command
    /// </summary>
    public class GetData : Iso7816.ApduCommand
    {
        public enum GetDataDataType : byte
        {
            Uid = 0x00,
            HistoricalBytes = 0x01 // Returned data excludes CRC
        }
        public GetDataDataType Type
        {
            set { base.P1 = (byte)value; }
            get { return (GetDataDataType)base.P1; }
        }
        public GetData(GetDataDataType type)
            : base((byte)Iso7816.Cla.ReservedForPts, (byte)Pcsc.Ins.GetData, (byte)type, 0, null, 0)
        {
        }
    }
    /// <summary>
    /// PCSC LoadKeys command
    /// </summary>
    public class LoadKeys : Iso7816.ApduCommand
    {
        public enum LoadKeysKeyType : byte
        {
            CardKey = 0x00,
            ReaderKey = 0x80,

            Mask = 0x80,
        }
        public enum LoadKeysTransmissionType : byte
        {
            Plain = 0x00,
            Secured = 0x40,

            Mask = 0x40,
        }
        public enum LoadKeysStorageType : byte
        {
            Volatile = 0x00,
            NonVolatile = 0x20,

            Mask = 0x20,
        }
        public LoadKeysKeyType KeyType
        {
            set { base.P1 = (byte)((base.P1 & ~(byte)LoadKeysKeyType.Mask) | (byte)(value & LoadKeysKeyType.Mask)); }
            get { return (LoadKeysKeyType)(base.P1 & (byte)LoadKeysKeyType.Mask); }
        }
        public LoadKeysTransmissionType TransmissionType
        {
            set { base.P1 = (byte)((base.P1 & ~(byte)LoadKeysTransmissionType.Mask) | (byte)(value & LoadKeysTransmissionType.Mask)); }
            get { return (LoadKeysTransmissionType)(base.P1 & (byte)LoadKeysTransmissionType.Mask); }
        }
        public LoadKeysStorageType StorageType
        {
            set { base.P1 = (byte)((base.P1 & ~(byte)LoadKeysStorageType.Mask) | (byte)(value & LoadKeysStorageType.Mask)); }
            get { return (LoadKeysStorageType)(base.P1 & (byte)LoadKeysStorageType.Mask); }
        }
        public byte ReaderKeyNumber
        {
            set { base.P1 = (byte)((base.P1 & 0xF0) | (byte)(value & 0x0F)); }
            get { return (byte)(base.P1 & 0x0F); }
        }
        public byte KeyNumber
        {
            set { base.P2 = value; }
            get { return base.P2; }
        }
        public byte[] KeyData
        {
            set { base.CommandData = value; }
            get { return base.CommandData; }
        }
        public LoadKeys(LoadKeysKeyType keyType, byte? readerKeyNumber, LoadKeysTransmissionType transmissionType, LoadKeysStorageType storageType, byte keyNumber, byte[] keyData)
            : base((byte)Iso7816.Cla.ReservedForPts, (byte)Pcsc.Ins.LoadKeys, (byte)((byte)keyType | (byte)transmissionType | (byte)storageType | (readerKeyNumber ?? 0)), keyNumber, keyData, null)
        {
        }
    }
    /// <summary>
    /// PCSC GeneralAuthenticate command
    /// </summary>
    public class GeneralAuthenticate : Iso7816.ApduCommand
    {
        public enum GeneralAuthenticateKeyType : byte
        {
            MifareKeyA = 0x60,
            PicoTagPassKeyB = 0x61
        }
        public enum GeneralAuthenticateVersionNumber : byte
        {
            VersionOne = 0x01
        }
        public GeneralAuthenticateVersionNumber VersionNumber
        {
            set { base.CommandData[0] = (byte)value; }
            get { return (GeneralAuthenticateVersionNumber)base.CommandData[0]; }
        }
        public ushort Address
        {
            set 
            {
                base.CommandData[1] = (byte)(value >> 8);
                base.CommandData[2] = (byte)(value & 0x00FF);
            }
            get { return (ushort)((base.CommandData[1] << 8) | base.CommandData[2]); }
        }
        public byte KeyType
        {
            set { base.CommandData[3] = value; }
            get { return base.CommandData[3]; }
        }
        public byte KeyNumber
        {
            set { base.CommandData[4] = value; }
            get { return base.CommandData[4]; }
        }
        public GeneralAuthenticate(GeneralAuthenticateVersionNumber version, ushort address, GeneralAuthenticateKeyType keyType, byte keyNo)
            : base((byte)Iso7816.Cla.ReservedForPts, (byte)Pcsc.Ins.GeneralAuthenticate, 0, 0, new byte[5] { (byte)version, (byte)(address >> 8), (byte)(address & 0x00FF), (byte)keyType, keyNo }, null)
        {
        }
    }
    /// <summary>
    /// PCSC ReadBinary command
    /// </summary>
    public class ReadBinary : Iso7816.ApduCommand
    {
        public ReadBinary(ushort address, byte? expectedReturnBytes)
            : base((byte)Iso7816.Cla.ReservedForPts, (byte)Pcsc.Ins.ReadBinary, 0, 0, null, expectedReturnBytes)
        {
            this.Address = address;
        }

        public ushort Address
        {
            set
            {
                base.P1 = (byte)(value >> 8);
                base.P2 = (byte)(value & 0x00FF);
            }
            get { return (ushort)((base.P1 << 8) | base.P2); }
        }
    }
    /// <summary>
    /// PCSC Updatebinary Command
    /// </summary>
    public class UpdateBinary : Iso7816.ApduCommand
    {
        public UpdateBinary(ushort address, byte[] dataToWrite)
            : base((byte)Iso7816.Cla.ReservedForPts, (byte)Pcsc.Ins.UpdateBinary, 0, 0, dataToWrite, null)
        {
            this.Address = address;
        }

        public ushort Address
        {
            set
            {
                base.P1 = (byte)(value >> 8);
                base.P2 = (byte)(value & 0x00FF);
            }
            get { return (ushort)((base.P1 << 8) | base.P2); }
        }
    }
    /// <summary>
    /// PCSC GetUid command
    /// </summary>
    public class GetUid : GetData
    {
        public GetUid()
            : base(GetData.GetDataDataType.Uid)
        {
        }
    }
    /// <summary>
    /// PCSC GetHistoricalBytes command
    /// </summary>
    public class GetHistoricalBytes : GetData
    {
        public GetHistoricalBytes()
            : base(GetData.GetDataDataType.HistoricalBytes)
        {
        }
    }
    /// <summary>
    /// PCSC ManageSession command
    /// </summary>
    public class ManageSession : Iso7816.ApduCommand
    {
        public enum DataObjectType
        {
            Version = 0x80,
            StartTransparentSession = 0x81,
            EndTransparentSession = 0x82,
            Timer = 0x5F46,
        }
        public ManageSession(byte[] dataObjects)
            : base((byte)Iso7816.Cla.ReservedForPts, (byte)Iso7816.Ins.Envelope, 0x00, 0x00, dataObjects, null)
        {
            ApduResponseType = typeof(ManageSessionResponse);
        }
    }
    /// <summary>
    /// PCSC TransparentExchange command
    /// </summary>
    public class TransparentExchange : Iso7816.ApduCommand
    {
        public enum DataObjectType
        {
            Transceive = 0x95,
            Timer = 0x5F46,
        }
        public TransparentExchange(byte[] dataObjects)
            : base((byte)Iso7816.Cla.ReservedForPts, (byte)Iso7816.Ins.Envelope, 0x00, 0x01, dataObjects, null)
        {
            ApduResponseType = typeof(TransparentExchangeResponse);
        }
    }
    /// <summary>
    /// PCSC IncrementDecrement command
    /// </summary>
    public class IncrementDecrement : Iso7816.ApduCommand
    {
        public enum DataObjectType
        {
            Increment = 0xA0,
            Decrement = 0xA1
        }
        public IncrementDecrement(byte[] dataObjects)
            : base((byte)Iso7816.Cla.ReservedForPts, (byte)Iso7816.Ins.Envelope, 0x00, 0x03, dataObjects, null)
        {
        }
    }
    /// <summary>
    /// PCSC Apdu response
    /// </summary>
    public class ApduResponse : Iso7816.ApduResponse
    {
        public ApduResponse()
            : base()
        {
        }
    }
    /// <summary>
    /// PCSC ManageSession response
    /// </summary>
    public class ManageSessionResponse : Iso7816.ApduResponse
    {
        public enum DataObjectType
        {
            GenericErrorStatus = 0xC0,
            Version = 0x80,
        }
        public ManageSessionResponse()
            : base()
        {
        }
        public override bool Succeeded
        {
            get
            {
                if (base.Succeeded)
                {
                    byte[] value = ExtractTlvDataObject(new byte[1] { (byte)DataObjectType.GenericErrorStatus });

                    if (value.Length != 3)
                        throw new System.InvalidOperationException("Invalid value size for TLV response");

                    return (value[0] == 0x00 && value[1] == 0x90 && value[2] == 0x00); // XX SW1 SW2 XX=Number of bad data object in the APDU
                }

                return false;
            }
        }
        public char Version
        {
            get
            {
                byte[] value = ExtractTlvDataObject(new byte[1] { (byte)DataObjectType.Version });

                if (value.Length != 1)
                    throw new System.InvalidOperationException("Invalid value size for TLV response");

                return System.BitConverter.ToChar(value, 0);
            }
        }
    }
    /// <summary>
    /// PCSC TransparentExchange response
    /// </summary>
    public class TransparentExchangeResponse : Iso7816.ApduResponse
    {
        public enum DataObjectType
        {
            GenericErrorStatus = 0xC0,
            ResponseStatus = 0x96,
            IccResponse = 0x97,
        }
        public TransparentExchangeResponse()
            : base()
        {
        }
        public override bool Succeeded
        {
            get
            {
                if (base.Succeeded)
                {
                    byte[] value = ExtractTlvDataObject(new byte[1] { (byte)DataObjectType.GenericErrorStatus });

                    if (value.Length != 3)
                        throw new System.InvalidOperationException("Invalid value size for TLV response");

                    return (value[0] == 0x00 && value[1] == 0x90 && value[2] == 0x00); // XX SW1 SW2 XX=Number of bad data object in the APDU
                }

                return false;
            }
        }
        public ushort ResponseStatus
        {
            get
            {
                byte[] value = ExtractTlvDataObject(new byte[1] { (byte)DataObjectType.ResponseStatus });

                if (value.Length != 2)
                    throw new System.InvalidOperationException("Invalid value size for TLV response");

                return System.BitConverter.ToUInt16(value, 0);
            }
            
        }
        public byte[] IccResponse
        {
            get
            {
                return ExtractTlvDataObject(new byte[1] { (byte)DataObjectType.IccResponse });
            }
        }
    }
}
