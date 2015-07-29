#include "pch.h"
#include "Account.h"

using namespace Platform;

String^ SDKTemplate::Account::Name::get()
{
    return m_Name;
}

void SDKTemplate::Account::Name::set(String^ value)
{
    m_Name = value;
}