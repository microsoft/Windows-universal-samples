// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

namespace WinRTComponent
{
    [Windows::Foundation::Metadata::AllowForWebAttribute]
    public ref class PrimeFoundEventArgs sealed
    {
    internal:
        PrimeFoundEventArgs(int prime) : m_prime(prime) { }
    public:
        property int Prime { int get() { return m_prime; } };
        property bool Cancel { bool get() { return m_cancel; }
                               void set(bool value) { m_cancel = value; } }

    private:
        const int m_prime;
        bool m_cancel = false;
    };

    [Windows::Foundation::Metadata::AllowForWebAttribute]
    public ref class WinRTSharedObject sealed
    {
    public:
        WinRTSharedObject();

#pragma warning(suppress:4462) // We manually marshal this interface.
        using PrimeFoundHandler = Windows::Foundation::TypedEventHandler<WinRTSharedObject^, PrimeFoundEventArgs^>;

        event PrimeFoundHandler^ PrimeFound
        {
            Windows::Foundation::EventRegistrationToken add(PrimeFoundHandler^ handler);
            void remove(Windows::Foundation::EventRegistrationToken token);
        internal:
            void raise(WinRTSharedObject^ sender, PrimeFoundEventArgs^ args);
        }
        void CheckPrimesInRange();
        bool IsPrime(int number);

        void setStartNumber(int num);
        void setEndNumber(int num);

    private:
        int startNumber;
        int endNumber;

        event Windows::Foundation::EventHandler<Platform::Object^>^ m_primeFoundAgile;
    };
}
