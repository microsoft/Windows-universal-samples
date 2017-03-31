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
using MicrosoftPassport.Server.Data;
using System;
using System.Net.Http;
using System.Security.Cryptography;
using System.Web.Http;

namespace MicrosoftPassport.Server.Controllers
{
    public class AuthenticationController : ApiController
    {
        [HttpPost]
        public string RequestChallenge(UserAndKeyHintMessage message)
        {
            try
            {
                byte[] challengeBuffer = new byte[16];
                // Generate Random nonce
                RNGCryptoServiceProvider rng = new RNGCryptoServiceProvider();
                rng.GetBytes(challengeBuffer);

                DAL.Instance.SetActiveChallenge(message.UserId, message.PublicKeyHint, challengeBuffer);
                return Convert.ToBase64String(challengeBuffer);
            }
            catch (Exception)
            {
                return string.Empty;
            }
        }

        [HttpPost]
        public bool SubmitResponse(SignatureMessage message)
        {
            bool retval = false;

            try
            {
                // Use the public key hint to identify which public key to use.
                byte[] challenge;
                byte[] publicKey = DAL.Instance.GetPublicKeyForUser(message.UserId, message.PublicKeyHint, out challenge);

                CngKey pubCngKey = new SubjectPublicKeyInfo(publicKey).GetPublicKey();

                // Validate that the original challenge was signed using the corresponding private key.
                using (RSACng pubKey = new RSACng(pubCngKey))
                {
                    byte[] signature = Convert.FromBase64String(message.Signature);
                    retval = pubKey.VerifyData(challenge, signature, HashAlgorithmName.SHA256, RSASignaturePadding.Pkcs1);
                }
            }
            catch (Exception)
            {
            }

            // Return a boolean response to the client
            // TODO:  What should actually be returned is an authentication token (OAuth or equivalent)
            // that the client uses for subsequent authentication requests
            return retval;
        }

        [HttpPost]
        public bool Register(RegisterMessage message)
        {
            bool retval = false;

            if (string.IsNullOrEmpty(message.UserId) ||
                string.IsNullOrEmpty(message.PublicKey))
            {
                return false;
            }

            try
            {
                byte[] publicKeyBuffer = Convert.FromBase64String(message.PublicKey);

                // Hash the public key.
                SHA256 hashalg = SHA256.Create();
                byte[] publicKeyHashBuffer = hashalg.ComputeHash(publicKeyBuffer);

                string publicKeyHash = Convert.ToBase64String(publicKeyHashBuffer);

                retval = DAL.Instance.RegisterUser(message.UserId, publicKeyBuffer, publicKeyHash, message.DeviceName);
            }
            catch (Exception)
            {
            }
            return retval;
        }

        [HttpPost]
        public bool RemoveRegisteredKey(UserAndKeyHintMessage message)
        {
            bool retval = false;
            try
            {
                retval = DAL.Instance.RemoveRegisteredKey(message.UserId, message.PublicKeyHint);
            }
            catch (Exception)
            {
            }
            return retval;
        }
    }
}
