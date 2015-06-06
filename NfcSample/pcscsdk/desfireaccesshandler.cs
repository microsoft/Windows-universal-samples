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
using System.Threading.Tasks;
using Windows.Devices.SmartCards;

using Pcsc;

namespace Desfire
{
    /// <summary>
    /// Access handler class for Desfire based ICC. It provides wrappers for different Desfire 
    /// commands
    /// </summary>
    public class AccessHandler
    {
        /// <summary>
        /// connection object to smart card
        /// </summary>
        private SmartCardConnection connectionObject { set; get; }
        /// <summary>
        /// Desfire command APDU
        /// </summary>
        private DesfireCommand desfireCommand { set; get; }
        /// <summary>
        /// Desfire response APDU
        /// </summary>
        private DesfireResponse desfireResponse { set; get; }
        /// <summary>
        /// Class constructor
        /// </summary>
        /// <param name="ScConnection">
        /// connection object to a Desfire ICC
        /// </param>
        public AccessHandler(SmartCardConnection ScConnection)
        {
            connectionObject = ScConnection;

            desfireCommand = new DesfireCommand();
        }
        /// <summary>
        /// Read card details commands
        /// </summary>
        /// <returns>
        /// returns Desfire CardDetails object
        /// </returns>
        public async Task<CardDetails> ReadCardDetailsAsync()
        {
            desfireCommand.Command = (byte) DesfireCommand.CommandType.GetVersion;
            desfireCommand.Data = null;

            DesfireResponse desfireRes = await connectionObject.TransceiveAsync(desfireCommand) as DesfireResponse;

            if (!desfireRes.SubsequentFrame || desfireRes.ResponseData.Length != 7)
            {
                return null;
            }

            CardDetails card = new CardDetails();

            card.HardwareVendorID = desfireRes.ResponseData[0];
            card.HardwareType = desfireRes.ResponseData[1];
            card.HardwareSubType = desfireRes.ResponseData[2];
            card.HardwareMajorVersion = desfireRes.ResponseData[3];
            card.HardwareMinorVersion = desfireRes.ResponseData[4];
            card.HardwareStorageSize = desfireRes.ResponseData[5];
            card.HardwareProtocolType = desfireRes.ResponseData[6];

            desfireCommand.Command = (byte)DesfireCommand.CommandType.GetAdditionalFrame;
            desfireRes = await connectionObject.TransceiveAsync(desfireCommand) as DesfireResponse;

            if (!desfireRes.SubsequentFrame || desfireRes.ResponseData.Length != 7)
            {
                // Not expected
                return null;
            }
            card.SoftwareVendorID = desfireRes.ResponseData[0];
            card.SoftwareType = desfireRes.ResponseData[1];
            card.SoftwareSubType = desfireRes.ResponseData[2];
            card.SoftwareMajorVersion = desfireRes.ResponseData[3];
            card.SoftwareMinorVersion = desfireRes.ResponseData[4];
            card.SoftwareStorageSize = desfireRes.ResponseData[5];
            card.SoftwareProtocolType = desfireRes.ResponseData[6];

            desfireRes = await connectionObject.TransceiveAsync(desfireCommand) as DesfireResponse;

            if (!desfireRes.Succeeded || desfireRes.ResponseData.Length != 14)
            {
                // Not expected
                return null;
            }

            card.UID = new byte[7];
            System.Buffer.BlockCopy(desfireRes.ResponseData, 0, card.UID, 0, 7);

            card.ProductionBatchNumber = new byte[5];
            System.Buffer.BlockCopy(desfireRes.ResponseData, 7, card.ProductionBatchNumber, 0, 5);

            card.WeekOfProduction = desfireRes.ResponseData[12];
            card.YearOfProduction = desfireRes.ResponseData[13];

            return card;
        }
        /// <summary>
        /// Select ICC application by AID
        /// </summary>
        /// <param name="aid">
        /// application id
        /// </param>
        public async Task SelectApplicationAsync(byte[] aid)
        {
            if (aid.Length != 3)
            {
                throw new NotSupportedException();
            }

            desfireCommand.Command = (byte)DesfireCommand.CommandType.SelectApplication;
            desfireCommand.Data = aid;

            DesfireResponse desfireRes = await connectionObject.TransceiveAsync(desfireCommand) as DesfireResponse;

            if (!desfireResponse.Succeeded)
            {
                throw new Exception("Failure selecting application, SW=" + desfireResponse.SW + " (" + desfireResponse.SWTranslation + ")");
            }
        }
        /// <summary>
        /// Read data command
        /// </summary>
        /// <param name="fileNumber">
        /// </param>
        /// <param name="offset">
        /// </param>
        /// <param name="length">
        /// </param>
        /// <returns>
        /// byte array of read data
        /// </returns>
        public async Task<byte[]> ReadDataAsync(byte fileNumber, ulong offset, ulong length)
        {
            var args = new byte[7];
            args[0] = fileNumber;
            args[1] = (byte)(offset & 0xFF);
            args[2] = (byte)((offset >> 8) & 0xFF);
            args[3] = (byte)((offset >> 16) & 0xFF);
            args[4] = (byte)(length & 0xFF);
            args[5] = (byte)((length >> 8) & 0xFF);
            args[6] = (byte)((length >> 16) & 0xFF);

            desfireCommand.Command = (byte)DesfireCommand.CommandType.ReadData;
            desfireCommand.Data = args;

            DesfireResponse desfireRes = await connectionObject.TransceiveAsync(desfireCommand) as DesfireResponse;

            if (!desfireRes.Succeeded)
            {
                throw new Exception("Failure selecting application, SW=" + desfireResponse.SW + " (" + desfireResponse.SWTranslation + ")");
            }

            return desfireRes.ResponseData;
        }
        /// <summary>
        /// Read record comand
        /// </summary>
        /// <param name="fileNumber"></param>
        /// <param name="offset"></param>
        /// <param name="numberOfRecords"></param>
        /// <returns>
        /// byte array of read data
        /// </returns>
        public async Task<byte[]> ReadRecordAsync(byte fileNumber, ulong offset, ulong numberOfRecords)
        {
            var args = new byte[7];
            args[0] = fileNumber;
            args[1] = (byte)(offset & 0xFF);
            args[2] = (byte)((offset >> 8) & 0xFF);
            args[3] = (byte)((offset >> 16) & 0xFF);
            args[4] = (byte)(numberOfRecords & 0xFF);
            args[5] = (byte)((numberOfRecords >> 8) & 0xFF);
            args[6] = (byte)((numberOfRecords >> 16) & 0xFF);

            desfireCommand.Command = (byte)DesfireCommand.CommandType.ReadRecord;
            desfireCommand.Data = args;

            DesfireResponse desfireRes = await connectionObject.TransceiveAsync(desfireCommand) as DesfireResponse;

            if (desfireRes.BoundaryError)
            {
                // Boundary error, the record doesn't exist
                return null;
            }
            if (!desfireRes.Succeeded)
            {
                throw new Exception("Failure selecting application, SW=" + desfireResponse.SW + " (" + desfireResponse.SWTranslation + ")");
            }

            return desfireRes.ResponseData;
        }
    }
    /// <summary>
    /// Class hodls Desfire card details information
    /// </summary>
    public class CardDetails
    {
        public byte HardwareVendorID { get; set; }
        public byte HardwareType { get; set; }
        public byte HardwareSubType { get; set; }
        public byte HardwareMajorVersion { get; set; }
        public byte HardwareMinorVersion { get; set; }
        public byte HardwareStorageSize { get; set; }
        public byte HardwareProtocolType { get; set; }
        public byte SoftwareVendorID { get; set; }
        public byte SoftwareType { get; set; }
        public byte SoftwareSubType { get; set; }
        public byte SoftwareMajorVersion { get; set; }
        public byte SoftwareMinorVersion { get; set; }
        public byte SoftwareStorageSize { get; set; }
        public byte SoftwareProtocolType { get; set; }
        // 7 bytes
        public byte[] UID { get; set; }
        // 5 bytes
        public byte[] ProductionBatchNumber { get; set; }
        public byte WeekOfProduction { get; set; }
        public byte YearOfProduction { get; set; }
        public override string ToString()
        {
            return
                "HardwareVendorID = " + HardwareVendorID.ToString() + Environment.NewLine +
                "HardwareType = " + HardwareType.ToString() + Environment.NewLine +
                "HardwareSubType = " + HardwareSubType.ToString() + Environment.NewLine +
                "HardwareMajorVersion = " + HardwareMajorVersion.ToString() + Environment.NewLine +
                "HardwareMinorVersion = " + HardwareMinorVersion.ToString() + Environment.NewLine +
                "HardwareStorageSize = " + HardwareStorageSize.ToString() + Environment.NewLine +
                "HardwareProtocolType = " + HardwareProtocolType.ToString() + Environment.NewLine +
                "SoftwareVendorID = " + SoftwareVendorID.ToString() + Environment.NewLine +
                "SoftwareType = " + SoftwareType.ToString() + Environment.NewLine +
                "SoftwareSubType = " + SoftwareSubType.ToString() + Environment.NewLine +
                "SoftwareMajorVersion = " + SoftwareMajorVersion.ToString() + Environment.NewLine +
                "SoftwareMinorVersion = " + SoftwareMinorVersion.ToString() + Environment.NewLine +
                "SoftwareStorageSize = " + SoftwareStorageSize.ToString() + Environment.NewLine +
                "SoftwareProtocolType = " + SoftwareProtocolType.ToString() + Environment.NewLine +
                "UID = " + BitConverter.ToString(UID) + Environment.NewLine +
                "ProductionBatchNumber = " + BitConverter.ToString(ProductionBatchNumber) + Environment.NewLine +
                "WeekOfProduction = " + WeekOfProduction.ToString() + Environment.NewLine +
                "YearOfProduction = " + YearOfProduction.ToString() + Environment.NewLine;
        }
    }
}
