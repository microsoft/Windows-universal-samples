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


#include <xmllite.h>
#include <shcore.h>
#include <wrl.h>


#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>

#define ChkHr(stmt) do { hr = stmt; if (FAILED(hr)) return hr; } while(0)
