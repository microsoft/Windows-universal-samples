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
using Windows.Devices.Enumeration;

namespace SDKTemplate
{
    public partial class DeviceHelpers
    {
        // We use a DeviceWatcher instead of DeviceInformation.FindAllAsync because
        // the DeviceWatcher will let us see the devices as they are discovered,
        // whereas FindAllAsync returns results only after discovery is complete.

        public static async Task<T> GetFirstDeviceAsync<T>(string selector, Func<string, Task<T>> convertAsync)
            where T : class
        {
            var completionSource = new TaskCompletionSource<T>();
            var pendingTasks = new List<Task>();
            DeviceWatcher watcher = DeviceInformation.CreateWatcher(selector);

            watcher.Added += (DeviceWatcher sender, DeviceInformation device) =>
            {
                Func<string, Task> lambda = async (id) =>
                {
                    T t = await convertAsync(id);
                    if (t != null)
                    {
                        completionSource.TrySetResult(t);
                    }
                };
                pendingTasks.Add(lambda(device.Id));
            };

            watcher.EnumerationCompleted += async (DeviceWatcher sender, object args) =>
            {
                // Wait for completion of all the tasks we created in our "Added" event handler.
                await Task.WhenAll(pendingTasks);
                // This sets the result to "null" if no task was able to produce a device.
                completionSource.TrySetResult(null);
            };

            watcher.Start();

            // Wait for enumeration to complete or for a device to be found.
            T result = await completionSource.Task;

            watcher.Stop();

            return result;
        }
    }
}
