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
// Please refer to the Microsoft Passport and Windows Hello
// whitepaper on the Windows Dev Center for a complete
// explanation of Microsoft Passport and Windows Hello
// implementation: 
// http://go.microsoft.com/fwlink/p/?LinkId=522066
//*********************************************************
using MicrosoftPassport.Models;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;

namespace MicrosoftPassport.Server.Data
{
    public class DAL
    {
        public static DAL Instance { get; } = new DAL();

        // In this sample, we keep a list of users and devices in memory, this will be cleared when the
        // server is stopped. In a real-world scenario, you would store this information in a database.

        private class RegisteredUser
        {
            public string UserId;
            public byte[] PublicKey;
            public string PublicKeyHash;
            public string DeviceName;
            public byte[] ActiveChallenge;
        }
        private List<RegisteredUser> Users { get; } = new List<RegisteredUser>();

        private DAL()
        {
        }

        public bool RegisterUser(string userId, byte[] publicKey, string publicKeyHash, string deviceName)
        {
            Users.Add(new RegisteredUser
            {
                UserId = userId,
                PublicKey = publicKey,
                PublicKeyHash = publicKeyHash,
                DeviceName = deviceName
            });
            return true;
        }

        public bool RemoveRegisteredKey(string userId, string publicKeyHash)
        {
            Users.RemoveAll(u => u.UserId == userId && u.PublicKeyHash == publicKeyHash);
            return true;
        }

        public byte[] GetPublicKeyForUser(string userId, string publicKeyHash, out byte[] challenge)
        {
            RegisteredUser user = Users.FirstOrDefault(u => u.UserId == userId && u.PublicKeyHash == publicKeyHash);

            // Each challenge is valid only once. Prevent replay attack.
            challenge = user.ActiveChallenge;
            user.ActiveChallenge = null;

            return user.PublicKey;
        }

        public bool SetActiveChallenge(string userId, string publicKeyHash, byte[] challenge)
        {
            RegisteredUser user = Users.FirstOrDefault(u => u.UserId == userId && u.PublicKeyHash == publicKeyHash);
            user.ActiveChallenge = challenge;
            return true;
        }
    }
}