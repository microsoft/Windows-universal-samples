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

using System.Threading.Tasks;
using Windows.Devices.SmartCards;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Storage.Streams;

using Pcsc;
using System;

namespace Iso15693
{
    /// <summary>
    /// Access handler class for Iso15693 based ICC. It provides wrappers for different Iso15693 
    /// commands
    /// </summary>
    public class AccessHandler
    {
        /// <summary>
        /// connection object to smart card
        /// </summary>
        private SmartCardConnection connectionObject { set; get; }
        /// <summary>
        /// Class constructor
        /// </summary>
        /// <param name="ScConnection">
        /// connection object to a Iso15693 ICC
        /// </param>
        public AccessHandler(SmartCardConnection ScConnection)
        {
            connectionObject = ScConnection;
        }
        /// <summary>
        /// Wrapper method to read data from the Iso15693 card
        /// </summary>
        /// <returns>
        /// byte array of the read data
        /// </returns>
        public async Task<byte[]> ReadAsync(byte blockNumber)
        {
            var apduRes = await connectionObject.TransceiveAsync(new Iso15693.Read(blockNumber));

            if (!apduRes.Succeeded)
            {
                throw new Exception("Failure reading Iso15693 card, " + apduRes.ToString());
            }

            return apduRes.ResponseData;
        }
        /// <summary>
        /// Wrapper method to write data to the Iso15693 card
        /// </summary>
        public async Task WriteAsync(byte blockNumber, byte[] dataToWrite)
        {
            if (dataToWrite.Length != 4)
            {
                throw new InvalidOperationException("Invalid size of data to write");
            }

            var apduRes = await connectionObject.TransceiveAsync(new Iso15693.Write(blockNumber, dataToWrite));

            if (!apduRes.Succeeded)
            {
                throw new Exception("Failure writing Iso15693 card, " + apduRes.ToString());
            }
        }
        /// <summary>
        /// Wrapper method to perform transparent transceive data to the Iso15693 card
        /// </summary>
        /// <param name="commandData">
        /// The command to send to the Iso15693 card
        /// </param>
        /// <returns>
        /// byte array of the read data
        /// </returns>
        public async Task <byte[]> TransparentExchangeAsync(byte[] commandData)
        {
            byte[] responseData = await connectionObject.TransparentExchangeAsync(commandData);

            return responseData;
        }
        /// <summary>
        /// Wrapper method get the Iso15693 UID
        /// </summary>
        /// <returns>
        /// byte array UID
        /// </returns>
        public async Task<byte[]> GetUidAsync()
        {
            var apduRes = await connectionObject.TransceiveAsync(new Iso15693.GetUid());

            if (!apduRes.Succeeded)
            {
                throw new Exception("Failure getting UID of Iso15693 card, " + apduRes.ToString());
            }

            return apduRes.ResponseData;
        }
    }
}
