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
using System.Threading.Tasks;
using Windows.Devices.Sensors;
using Windows.Foundation.Metadata;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Altimeter";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Data Events", ClassType = typeof(Scenario1_DataEvents) },
            new Scenario() { Title = "Polling", ClassType = typeof(Scenario2_Polling) }
        };

        // This works around an issue in the Anniversary Update (1607) in which
        // Altimeter.GetDefault() returns a nonfunctional altimeter if the
        // system has no altimeter. This issue does not exist in other versions
        // of Windows 10, but the workaround is harmless to use even on versions
        // which do not have this problem. The workaround returns the default
        // altimeter only after we confirm that the system has a working altimeter.

        private static async Task<Altimeter> GetDefaultAltimeterWorkerAsync()
        {
            // This workaround is needed only on the Anniversary Update (universal contract 3).
            if (!ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 3) ||
                ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 4))
            {
                // The current system does not require the workaround.
                return Altimeter.GetDefault();
            }

            var tcs = new TaskCompletionSource<Altimeter>();

            string deviceSelector =
                // Find all interface classes for altimeter sensors
                "System.Devices.InterfaceClassGuid:=\"{0E903829-FF8A-4A93-97DF-3DCBDE402288}\"" +
                // ... that are present on the system
                " AND System.Devices.InterfaceEnabled:=System.StructuredQueryType.Boolean#True";

            var watcher = Windows.Devices.Enumeration.DeviceInformation.CreateWatcher(deviceSelector);

            watcher.Added += (s, e) => tcs.TrySetResult(Altimeter.GetDefault());
            watcher.EnumerationCompleted += (s, e) => tcs.TrySetResult(null);
            watcher.Start();
            Altimeter altimeter = await tcs.Task;
            watcher.Stop();
            return altimeter;
        }

        // Run this task only once, and cache the result.
        private static Lazy<Task<Altimeter>> defaultAltimeterLazy =
            new Lazy<Task<Altimeter>>(GetDefaultAltimeterWorkerAsync, System.Threading.LazyThreadSafetyMode.ExecutionAndPublication);

        public static Task<Altimeter> GetDefaultAltimeterAsync()
        {
            return defaultAltimeterLazy.Value;
        }
    }


    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
