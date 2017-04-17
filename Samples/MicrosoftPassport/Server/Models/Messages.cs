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
namespace MicrosoftPassport.Models
{
    public class RegisterMessage
    {
        public string UserId;
        public string PublicKey;
        public string DeviceName;
    }

    public class UserAndKeyHintMessage
    {
        public string UserId;
        public string PublicKeyHint;
    }

    public class SignatureMessage
    {
        public string UserId;
        public string Signature;
        public string PublicKeyHint;
    }
}
