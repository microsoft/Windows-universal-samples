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

using com.microsoft.Samples.SecureInterface;
using SDKTemplate;
using System;
using System.Threading.Tasks;
using Windows.Devices.AllJoyn;
using Windows.Foundation;

namespace AllJoynProducerExperiences
{
    /// <summary>
    /// The implementation of ISecureInterfaceService that will handle the concatenation method calls.
    /// </summary>
    class SecureInterfaceService : ISecureInterfaceService
    {
        // Method to handle calls to the Concatenate method.
        public IAsyncOperation<SecureInterfaceConcatenateResult> ConcatenateAsync(AllJoynMessageInfo info, string inStr1, string inStr2)
        {
            IAsyncAction asyncAction = MainPage.Current.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                MainPage.Current.NotifyUser(string.Format("Concatenation request recieved for \"{0}\" and \"{1}\".", inStr1, inStr2), NotifyType.StatusMessage);
            });

            Task<SecureInterfaceConcatenateResult> task = new Task<SecureInterfaceConcatenateResult>(() =>
            {
                if (AppData.IsUpperCaseEnabled)
                {
                    inStr1 = inStr1.ToUpper();
                    inStr2 = inStr2.ToUpper();
                }
                return SecureInterfaceConcatenateResult.CreateSuccessResult(inStr1 + inStr2);
            });

            task.Start();
            return task.AsAsyncOperation();
        }

        // Method to handle requests for the value of the IsUpperCaseEnabled property.
        public IAsyncOperation<SecureInterfaceGetIsUpperCaseEnabledResult> GetIsUpperCaseEnabledAsync(AllJoynMessageInfo info)
        {
            Task<SecureInterfaceGetIsUpperCaseEnabledResult> task = new Task<SecureInterfaceGetIsUpperCaseEnabledResult>(() =>
            {
                return SecureInterfaceGetIsUpperCaseEnabledResult.CreateSuccessResult(AppData.IsUpperCaseEnabled);
            });

            task.Start();
            return task.AsAsyncOperation();
        }

        // Method to handle requests to set the IsUpperCaseEnabled property.
        public IAsyncOperation<SecureInterfaceSetIsUpperCaseEnabledResult> SetIsUpperCaseEnabledAsync(AllJoynMessageInfo info, bool value)
        {
            Task<SecureInterfaceSetIsUpperCaseEnabledResult> task = new Task<SecureInterfaceSetIsUpperCaseEnabledResult>(() =>
            {
                AppData.IsUpperCaseEnabled = value;
                return SecureInterfaceSetIsUpperCaseEnabledResult.CreateSuccessResult();
            });

            task.Start();
            return task.AsAsyncOperation();
        }
    }
}
