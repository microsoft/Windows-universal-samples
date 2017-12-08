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

#include <collection.h>
#include <ppltasks.h>
#include <Windows.Foundation.Numerics.h>
#include <robuffer.h>
#include <wrl.h>

#include <Mfobjects.h>
#include <Mfapi.h>

#include <string>
#include "BufferHelper.h"

// Because this function is only used to check the size of an IBuffer - which uses a capacity of type int, not size_t - 
// we return a uint32_t here.
template<typename parentType, typename repeatedType, uint32_t declaredCount = 1>
uint32_t sizeof_composedStructure(uint32_t effectiveCount)
{
    return static_cast<uint32_t>(sizeof(parentType) + (static_cast<int32_t>(effectiveCount) - static_cast<int32_t>(declaredCount)) * sizeof(repeatedType));
}