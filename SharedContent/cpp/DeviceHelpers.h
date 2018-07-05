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

#pragma once

namespace SDKTemplate
{
    namespace DeviceHelpers
    {
        // We use a DeviceWatcher instead of DeviceInformation.FindAllAsync because
        // the DeviceWatcher will let us see the devices as they are discovered,
        // whereas FindAllAsync returns results only after discovery is complete.
        //
        // The convertAsync functional is passed a device ID (Platform::String^) and returns a
        // Concurrency::task<Something^>.
        template<typename Converter>
        auto GetFirstDeviceAsync(Platform::String^ selector, Converter convertAsync)
            -> decltype(convertAsync(nullptr))
        {
            using T = typename decltype(convertAsync(nullptr))::result_type;
            Concurrency::task_completion_event<T> completionEvent;
            auto pendingTasks = std::make_shared<std::vector<Concurrency::task<void>>>();
            Windows::Devices::Enumeration::DeviceWatcher^ watcher = Windows::Devices::Enumeration::DeviceInformation::CreateWatcher(selector);

            watcher->Added += ref new Windows::Foundation::TypedEventHandler<Windows::Devices::Enumeration::DeviceWatcher^, Windows::Devices::Enumeration::DeviceInformation^>(
                [completionEvent, pendingTasks, convertAsync](Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformation^ device)
            {
                auto task = convertAsync(device->Id).then([completionEvent](T t)
                {
                    if (t != nullptr)
                    {
                        completionEvent.set(t);
                    }
                });
                pendingTasks->push_back(task);
            });

            watcher->EnumerationCompleted += ref new Windows::Foundation::TypedEventHandler<Windows::Devices::Enumeration::DeviceWatcher^, Platform::Object^>(
                [completionEvent, pendingTasks](Windows::Devices::Enumeration::DeviceWatcher^ sender, Platform::Object^ args)
            {
                // Wait for completion of all the tasks we created in our "Added" event handler.
                Concurrency::when_all(pendingTasks->begin(), pendingTasks->end()).then([completionEvent]()
                {
                    // This sets the result to "nullptr" if no task was able to produce a device.
                    completionEvent.set(nullptr);
                });
            });

            watcher->Start();

            // Wait for enumeration to complete or for a device to be found, whichever comes first.
            // Start with a task created from IAsyncAction so that chained tasks run in the same apartment.
            return Concurrency::create_task(concurrency::create_async([] {})).then([completionEvent]()
            {
                return Concurrency::create_task(completionEvent);
            }).then([watcher](T result)
            {
                watcher->Stop();
                return result;
            });
        }
    }
}
