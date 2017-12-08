// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "pch.h"
#include "WinRTSharedObject.h"
#include <vector>

using namespace WinRTComponent;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;

WinRTSharedObject::WinRTSharedObject()
{
}

EventRegistrationToken WinRTSharedObject::PrimeFound::add(PrimeFoundHandler^ handler)
{
    // Since the WinRTSharedObject is shared between threads, we capture the context
    // at registration time so that the callback is made on the correct thread.
    return m_primeFoundAgile += ref new Windows::Foundation::EventHandler<Platform::Object^>(
        [handler](Platform::Object^ sender, Platform::Object^ args)
    {
        handler->Invoke(safe_cast<WinRTSharedObject^>(sender), safe_cast<PrimeFoundEventArgs^>(args));
    }, Platform::CallbackContext::Same);
}

void WinRTSharedObject::PrimeFound::remove(EventRegistrationToken token)
{
    m_primeFoundAgile -= token;
}

void WinRTSharedObject::PrimeFound::raise(WinRTSharedObject^ sender, PrimeFoundEventArgs^ args)
{
    m_primeFoundAgile(sender, args);
}

void WinRTSharedObject::CheckPrimesInRange()
{
    ThreadPool::RunAsync(ref new WorkItemHandler([this](IAsyncAction^ action)
    {
        for (int num = startNumber; num <= endNumber && action->Status != AsyncStatus::Canceled; num++)
        {
            if (IsPrime(num))
            {
                auto args = ref new PrimeFoundEventArgs(num);
                PrimeFound(this, args);
                // An event handler can set cancel=true to stop the computation.
                if (args->Cancel)
                {
                    break;
                }
            }
        }
    }), WorkItemPriority::Normal, WorkItemOptions::TimeSliced);
}

bool WinRTSharedObject::IsPrime(int n)
{
    if (n < 2)
    {
        return false;
    }
    for (int i = 2; i < n; ++i)
    {
        if ((n % i) == 0)
        {
            return false;
        }
    }
    return true;
}

void WinRTSharedObject::setStartNumber(int num)
{
    startNumber = num;
}

void WinRTSharedObject::setEndNumber(int num)
{
    endNumber = num;
}