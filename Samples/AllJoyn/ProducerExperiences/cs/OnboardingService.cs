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

using org.alljoyn.Onboarding;
using SDKTemplate;
using System;
using System.Threading.Tasks;
using Windows.Devices.AllJoyn;
using Windows.Foundation;

namespace AllJoynProducerExperiences
{
    class OnboardingService : IOnboardingService
    {
        public static event EventHandler ConnectRequestRecieved;

        // Method to handle calls to the ConfigureWifi method.
        IAsyncOperation<OnboardingConfigureWiFiResult> IOnboardingService.ConfigureWiFiAsync(AllJoynMessageInfo info, string interfaceMemberSsid, string interfaceMemberPassphrase, short interfaceMemberAuthType)
        {
            IAsyncAction asyncAction = MainPage.Current.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                MainPage.Current.NotifyUser("Configure WiFi request received", NotifyType.StatusMessage);
            });

            Task<OnboardingConfigureWiFiResult> task = new Task<OnboardingConfigureWiFiResult>(() =>
            {
                AppData.OnboardingConfigureSsid = interfaceMemberSsid;
                AppData.OnboardingConfigurePassphrase = interfaceMemberPassphrase;
                AppData.OnboardingConfigureAuthType = interfaceMemberAuthType;
                AppData.OnboardingConfigurationState = (short)ConfigurationState.NotValidated;
                return OnboardingConfigureWiFiResult.CreateSuccessResult((short)ConfigureWiFiResultStatus.Concurrent);
            });

            task.Start();
            return task.AsAsyncOperation();
        }

        // Method to handle calls to the Connect method.
        IAsyncOperation<OnboardingConnectResult> IOnboardingService.ConnectAsync(AllJoynMessageInfo info)
        {
            Task<OnboardingConnectResult> task = new Task<OnboardingConnectResult>(() =>
            {
                EventHandler handler = ConnectRequestRecieved;
                if (handler != null)
                {
                    handler(new object(), new EventArgs());
                }

                return OnboardingConnectResult.CreateSuccessResult();
            });

            task.Start();
            return task.AsAsyncOperation();
        }

        // Method to handle calls to the Offboard method.
        IAsyncOperation<OnboardingOffboardResult> IOnboardingService.OffboardAsync(AllJoynMessageInfo info)
        {
            Task<OnboardingOffboardResult> task = new Task<OnboardingOffboardResult>(() =>
            {
                AppData.OnboardingConfigureSsid = null;
                AppData.OnboardingConfigurePassphrase = null;
                AppData.OnboardingConfigureAuthType = (short)OnboardingAuthenticationType.Any;
                AppData.OnboardingConfigurationState = (short)ConfigurationState.NotConfigured;
                return OnboardingOffboardResult.CreateSuccessResult();
            });

            task.Start();
            return task.AsAsyncOperation();
        }

        // Method to handle calls to the GetScanInfo method.
        IAsyncOperation<OnboardingGetScanInfoResult> IOnboardingService.GetScanInfoAsync(AllJoynMessageInfo info)
        {
            Task<OnboardingGetScanInfoResult> task = new Task<OnboardingGetScanInfoResult>(() =>
            {
                return OnboardingGetScanInfoResult.CreateSuccessResult(1, AppData.SampleNetworkScanList);
            });

            task.Start();
            return task.AsAsyncOperation();
        }

        // Method to handle requests for the value of the Version property.
        IAsyncOperation<OnboardingGetVersionResult> IOnboardingService.GetVersionAsync(AllJoynMessageInfo info)
        {
            Task<OnboardingGetVersionResult> task = new Task<OnboardingGetVersionResult>(() =>
            {
                return OnboardingGetVersionResult.CreateSuccessResult(1);
            });

            task.Start();
            return task.AsAsyncOperation();
        }

        // Method to handle requests for the value of the State property.
        IAsyncOperation<OnboardingGetStateResult> IOnboardingService.GetStateAsync(AllJoynMessageInfo info)
        {
            Task<OnboardingGetStateResult> task = new Task<OnboardingGetStateResult>(() =>
            {
                return OnboardingGetStateResult.CreateSuccessResult(AppData.OnboardingConfigurationState);
            });

            task.Start();
            return task.AsAsyncOperation();
        }

        // Method to handle requests for the value of the LastError property.
        IAsyncOperation<OnboardingGetLastErrorResult> IOnboardingService.GetLastErrorAsync(AllJoynMessageInfo info)
        {
            Task<OnboardingGetLastErrorResult> task = new Task<OnboardingGetLastErrorResult>(() =>
            {
                OnboardingLastError lastError = new OnboardingLastError();
                lastError.Value1 = AppData.LastErrorCode;
                lastError.Value2 = AppData.LastErrorMessage;
                return OnboardingGetLastErrorResult.CreateSuccessResult(lastError);
            });

            task.Start();
            return task.AsAsyncOperation();
        }
    }
}
