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
#include "pch.h"

namespace winrt
{
    hstring to_hstring(Windows::Devices::Enumeration::DevicePairingResultStatus status);
    hstring to_hstring(Windows::Devices::Enumeration::DeviceUnpairingResultStatus status);
    hstring to_hstring(Windows::Devices::Enumeration::DeviceInformationKind kind);
}

namespace winrt::SDKTemplate
{
    template<typename T>
    T* FromInspectable(Windows::Foundation::IInspectable const& insp)
    {
        // Note that this a reference to the T inside the IInspectable,
        // so the caller is responsible for keeping the IInspectable alive.
        return insp ? get_self<T>(insp.as<typename T::class_type>()) : nullptr;
    }

    // RAII type for ensuring a deferral is completed.
    template<typename T>
    struct ensure_complete
    {
        ensure_complete(T t) : deferral(std::move(t)) { }
        ~ensure_complete() { if (deferral) deferral.Complete(); }

        ensure_complete(const ensure_complete&) = delete;
        ensure_complete& operator=(const ensure_complete&) = delete;

        ensure_complete(ensure_complete&& other) = default;
        ensure_complete& operator=(ensure_complete&& other) = default;
    };

    template<typename T>
    struct SimpleTaskCompletionSource : SimpleTaskCompletionSource<void>
    {       
        void SetResult(T value)
        {
            if (!result)
            {
                result = std::move(value);
                SimpleTaskCompletionSource<void>::SetResult();
            }
        }

        Windows::Foundation::IAsyncOperation<T> Task()
        {
            co_await SimpleTaskCompletionSource<void>::Task();
            co_return result.value();
        }
    private:
        std::optional<T> result{};
    };

    template<>
    struct SimpleTaskCompletionSource<void>
    {
        SimpleTaskCompletionSource()
        {
            check_bool(static_cast<bool>(completeEvent));
        }

        void SetResult()
        {
            SetEvent(completeEvent.get());
        }

        Windows::Foundation::IAsyncAction Task()
        {
            handle localEvent;
            check_bool(DuplicateHandle(GetCurrentProcess(), completeEvent.get(), GetCurrentProcess(), localEvent.put(), 0, FALSE, DUPLICATE_SAME_ACCESS));
            co_await resume_on_signal(localEvent.get());
        }
    private:
        handle completeEvent{ CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE) };
    };
}