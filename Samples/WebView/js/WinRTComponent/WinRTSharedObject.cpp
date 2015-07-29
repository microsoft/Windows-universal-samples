// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "pch.h"
#include "WinRTSharedObject.h"
#include <vector>

using namespace WinRTComponent;
using namespace std;

WinRTSharedObject::WinRTSharedObject()
{
}

void WinRTSharedObject::CheckPrimesInRange()
{
    vector<int> numsVector;

    for (int num = startNumber; num <= endNumber; num++)
    {
        if (IsPrime(num))
        {
            numsVector.push_back(num);
            NotifyAppOfPrimes(num);
        }
    }

    int* numsArray = &numsVector[0];
    //return numsArray;
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