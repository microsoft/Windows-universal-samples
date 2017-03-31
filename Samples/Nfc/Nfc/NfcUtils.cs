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
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.Devices.SmartCards;
using System.Diagnostics;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Storage.Streams;
using Windows.ApplicationModel;

namespace NfcSample
{
    public enum NotifyType
    {
        StatusMessage,
        ErrorMessage
    };

    public class NfcUtils
    {
        public static async Task<IBackgroundTaskRegistration> GetOrRegisterHceBackgroundTask(string bgTaskName, string taskEntryPoint, SmartCardTriggerType triggerType)
        {
            try
            {
                // Check if there's an existing background task already registered
                var bgTask = (from t in BackgroundTaskRegistration.AllTasks
                              where t.Value.Name.Equals(bgTaskName)
                              select t.Value).SingleOrDefault();
                if (bgTask != null)
                {
                    LogMessage("Background task already registered", NotifyType.StatusMessage);
                }
                else
                {
                    if (!(await DoBackgroundRequestAccess()))
                    {
                        LogMessage("Background task access denied, task won't fire", NotifyType.ErrorMessage);
                        throw new Exception("Failed to get background access");
                    }

                    var taskBuilder = new BackgroundTaskBuilder();
                    taskBuilder.Name = bgTaskName;
                    taskBuilder.TaskEntryPoint = taskEntryPoint;
                    taskBuilder.SetTrigger(new SmartCardTrigger(triggerType));
                    bgTask = taskBuilder.Register();
                    LogMessage("Background task registered", NotifyType.StatusMessage);
                }

                bgTask.Completed += BgTask_Completed;
                bgTask.Progress += BgTask_Progress;

                return bgTask;
            }
            catch (Exception ex)
            {
                LogMessage("Failed to register background task: " + ex.ToString(), NotifyType.StatusMessage);
                throw;
            }
        }

        private static async Task<bool> DoBackgroundRequestAccess()
        {
            String appVersion = String.Format("{0}.{1}.{2}.{3}",
                    Package.Current.Id.Version.Build,
                    Package.Current.Id.Version.Major,
                    Package.Current.Id.Version.Minor,
                    Package.Current.Id.Version.Revision);

            if ((string)Windows.Storage.ApplicationData.Current.LocalSettings.Values["AppVersion"] != appVersion)
            {
                // Our app has been updated
                Windows.Storage.ApplicationData.Current.LocalSettings.Values["AppVersion"] = appVersion;

                // Call RemoveAccess
                BackgroundExecutionManager.RemoveAccess();
            }

            BackgroundAccessStatus status = await BackgroundExecutionManager.RequestAccessAsync();

            return status == BackgroundAccessStatus.AlwaysAllowed
                || status == BackgroundAccessStatus.AllowedSubjectToSystemPolicy;
        }

        private static void BgTask_Progress(BackgroundTaskRegistration sender, BackgroundTaskProgressEventArgs args)
        {
            LogMessage("BgTask Progress: " + args.Progress, NotifyType.StatusMessage);
        }

        private static void BgTask_Completed(BackgroundTaskRegistration sender, BackgroundTaskCompletedEventArgs args)
        {
            LogMessage("BgTask Completed", NotifyType.StatusMessage);
        }

        public static async Task<SmartCardAppletIdGroupRegistration> RegisterAidGroup(string displayName, IList<IBuffer> appletIds, SmartCardEmulationCategory? emulationCategory, SmartCardEmulationType emulationType, bool automaticEnablement)
        {
            try
            {
                var cardAidGroup = new SmartCardAppletIdGroup(
                    displayName, 
                    appletIds, 
                    (SmartCardEmulationCategory)emulationCategory, 
                    emulationType);
                cardAidGroup.AutomaticEnablement = automaticEnablement;

                var reg = await SmartCardEmulator.RegisterAppletIdGroupAsync(cardAidGroup);
                LogMessage("AID group successfully registered", NotifyType.StatusMessage);

                return reg;
            }
            catch (Exception ex)
            {
                LogMessage("Failed to register AID group: " + ex.ToString(), NotifyType.StatusMessage);
                throw;
            }
        }

        public static async Task<SmartCardAppletIdGroupRegistration> GetAidGroupById(Guid id)
        {
            try
            {
                // Find registration
                var reg = (from r in await SmartCardEmulator.GetAppletIdGroupRegistrationsAsync()
                           where r.Id == id
                           select r).SingleOrDefault();
                if (reg == null)
                {
                    LogMessage("No matching AID group found for specified ID", NotifyType.ErrorMessage);
                }

                return reg;
            }
            catch (Exception ex)
            {
                LogMessage("Failed to get AID group by ID: " + ex.ToString(), NotifyType.StatusMessage);
                throw;
            }
        }

        public static async Task<SmartCardActivationPolicyChangeResult> SetCardActivationPolicy(SmartCardAppletIdGroupRegistration reg, SmartCardAppletIdGroupActivationPolicy policy)
        {            
            LogMessage("Previous AID group activation policy: " + reg.ActivationPolicy, NotifyType.StatusMessage);

            // Now try to set the policy on the card, this may pop UI to the user asking them to confirm
            var res = await reg.RequestActivationPolicyChangeAsync(policy);
            if (res == SmartCardActivationPolicyChangeResult.Allowed)
            {
                LogMessage("AID group successfully set to " + policy.ToString(), NotifyType.StatusMessage);
            }
            else
            {
                LogMessage("AID group policy change was disallowed (perhaps due to user declining the UI popup)", NotifyType.ErrorMessage);
            }

            return res;
        }

        public static async Task<bool> CheckHceSupport()
        {
            // Check if the SmartCardEmulator API exists on this currently running SKU of Windows
            if (!Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.Devices.SmartCards.SmartCardEmulator"))
            {
                LogMessage("This SKU of Windows does not support NFC card emulation, only phones/mobile devices support NFC card emulation", NotifyType.ErrorMessage);
                return false;
            }

            // Check if any NFC emulation is supported on this device
            var sce = await SmartCardEmulator.GetDefaultAsync();
            if (sce == null)
            {
                LogMessage("NFC card emulation is not supported on this device, probably the device does not have NFC at all", NotifyType.ErrorMessage);
                return false;
            }

            // Check if the NFC emulation support on this device includes HCE
            if (!sce.IsHostCardEmulationSupported())
            {
                LogMessage("This device's NFC does not support HCE-mode", NotifyType.ErrorMessage);
                return false;
            }
            return true;
        }

        public static async void LaunchNfcPaymentsSettingsPage()
        {
            await Windows.System.Launcher.LaunchUriAsync(new Uri("ms-settings-nfctransactions:"));
        }

        public static async void LaunchNfcProximitySettingsPage()
        {
            await Windows.System.Launcher.LaunchUriAsync(new Uri("ms-settings-proximity:"));
        }

        public static byte[] HexStringToBytes(string hexString)
        {
            if (hexString.Length % 2 != 0)
            {
                throw new ArgumentException();
            }  
            byte[] bytes = new byte[hexString.Length / 2];
            for (int i = 0; i < hexString.Length; i += 2)
            {
                bytes[i / 2] = byte.Parse(hexString.Substring(i, 2), System.Globalization.NumberStyles.HexNumber);
            }
            return bytes;
        }

        public static void LogMessage(string message, NotifyType type = NotifyType.StatusMessage)
        {
            Debug.WriteLine(message);
            SDKTemplate.MainPage.Current.NotifyUser(message, type);
        }
    }
}
