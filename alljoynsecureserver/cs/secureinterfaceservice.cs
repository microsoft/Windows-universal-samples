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

using org.alljoyn.Bus.Samples.Secure;
using System;
using System.Threading.Tasks;
using Windows.Devices.AllJoyn;
using Windows.Foundation;

namespace AllJoynSecureServer
{
    /// <summary>
    /// The implementation of ISecureInterfaceService that will handle the concatenation method calls.
    /// </summary>
    class SecureInterfaceService : ISecureInterfaceService
    {
        // Function to handle calls to the Cat method.
        public IAsyncOperation<SecureInterfaceCatResult> CatAsync(AllJoynMessageInfo info, string inStr1, string inStr2)
        {
            Task<SecureInterfaceCatResult> task = new Task<SecureInterfaceCatResult>(() =>
            {
                return SecureInterfaceCatResult.CreateSuccessResult(inStr1 + inStr2);
            });

            task.Start();
            return task.AsAsyncOperation();
        }
    }
}
